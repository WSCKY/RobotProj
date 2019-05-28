/*
 * lsm9ds1.c
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#include "lsm9ds1.h"

static uint8_t imu_configured = 0;

static IMU_RAW imu_raw_data;
#if FREERTOS_ENABLED
static QueueHandle_t imu_queue = NULL;
#endif /* FREERTOS_ENABLED */

static uint8_t imu_tx_buffer[15] = {0};
static uint8_t imu_rx_buffer[15] = {0};
static uint8_t imu_mag_rx_buffer[7] = {0};

static void imu_write_reg_dma(uint8_t reg, uint8_t val, uint8_t id);
static void imu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id);
static void imu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id);

#if FREERTOS_ENABLED
static void imu_queue_create(void);
#endif /* FREERTOS_ENABLED */

uint16_t chip_id = 0xFFFF; /* (chip_id >> 8) = LSM9DS1_WHO_AM_I_VALUE */
/*
 * configure the IMU Sensor registers.
 */
int lsm9ds1_init(void)
{
  imu_configured = 0;
  /* initialize spi bus. */
  imu_spi_init();
  /* Communication with all registers of the device is performed using SPI max at 10MHz */
  imu_spi_config_rate(IMU_SPI_FAST_RATE);

  imu_read_reg_dma_util(LSM9DS1_WHO_AM_I, 1, (uint8_t *)&chip_id, IMU_SPI_DEV_ID_A);
  if((chip_id >> 8) != LSM9DS1_WHO_AM_I_VALUE) return -1;

  /* acceleration +/-8g @952Hz */
  imu_write_reg_dma(LSM9DS1_CTRL_REG6_XL, LSM9DS1_CTRL_REG6_XL_FS_XL_8G | LSM9DS1_CTRL_REG6_XL_ODR_XL_952HZ, IMU_SPI_DEV_ID_A);
  /* gyroscope +/-2000dps @952Hz */
  imu_write_reg_dma(LSM9DS1_CTRL_REG1_G, LSM9DS1_CTRL_REG1_G_FS_G_2000DPS | LSM9DS1_CTRL_REG1_G_ODR_G_952HZ, IMU_SPI_DEV_ID_G);
  /* gyroscope orientation */
  imu_write_reg_dma(LSM9DS1_ORIENT_CFG_G, LSM9DS1_ORIENT_CFG_G_SIGNY_G, IMU_SPI_DEV_ID_G);
  /* enable Accelerometer & Gyroscope data ready interrupt (INT2) */
  imu_write_reg_dma(LSM9DS1_INT2_CTRL, LSM9DS1_INT2_CTRL_INT2_DRDY_XL | LSM9DS1_INT2_CTRL_INT2_DRDY_G, IMU_SPI_DEV_ID_A);
  /* Active Accelerometer & Gyroscope Interrupt */
  imu_write_reg_dma(LSM9DS1_INT_GEN_SRC_XL, LSM9DS1_INT_GEN_SRC_XL_IA_XL, IMU_SPI_DEV_ID_A);
  imu_write_reg_dma(LSM9DS1_INT_GEN_SRC_G, LSM9DS1_INT_GEN_SRC_G_IA_G, IMU_SPI_DEV_ID_G);

  imu_delay(10);
  imu_read_reg_dma_util(LSM9DS1_WHO_AM_I_M, 1, (uint8_t *)&chip_id, IMU_SPI_DEV_ID_M);
  if((chip_id >> 8) != LSM9DS1_WHO_AM_I_M_VALUE) return -1;
  imu_delay(10);
  /* Magnetometer +/-4gauss, reboot memory content, reset register */
  imu_write_reg_dma(LSM9DS1_CTRL_REG2_M, LSM9DS1_CTRL_REG2_M_FS_4GAUSS | LSM9DS1_CTRL_REG2_M_REBOOT
		                               | LSM9DS1_CTRL_REG2_M_SOFT_RST, IMU_SPI_DEV_ID_M);
  imu_delay(10);
  /* Magnetometer Power-down mode, low-power mode */
  imu_write_reg_dma(LSM9DS1_CTRL_REG3_M, LSM9DS1_CTRL_REG3_M_LP | LSM9DS1_CTRL_REG3_M_MD_POWERDOWN2
		                               | LSM9DS1_CTRL_REG3_M_I2C_DISABLE, IMU_SPI_DEV_ID_M);
  imu_delay(10);
  /* Magnetometer 0.625Hz, Ultra-high performance mode(X/Y), Temperature compensation enable, Self-test enable */
  imu_write_reg_dma(LSM9DS1_CTRL_REG1_M, LSM9DS1_CTRL_REG1_M_DO_0p625HZ | LSM9DS1_CTRL_REG1_M_ST
  		                               | LSM9DS1_CTRL_REG1_M_TEMP_COMP | LSM9DS1_CTRL_REG1_M_OM_ULTRAHIGH, IMU_SPI_DEV_ID_M);
  imu_delay(50);
  /* Magnetometer +/-16gauss, reboot memory content, reset register */
  imu_write_reg_dma(LSM9DS1_CTRL_REG2_M, LSM9DS1_CTRL_REG2_M_FS_4GAUSS | LSM9DS1_CTRL_REG2_M_REBOOT
  		                               | LSM9DS1_CTRL_REG2_M_SOFT_RST, IMU_SPI_DEV_ID_M);
  imu_delay(10);
  /* Magnetometer power-down mode, low-power mode */
  imu_write_reg_dma(LSM9DS1_CTRL_REG3_M, LSM9DS1_CTRL_REG3_M_LP | LSM9DS1_CTRL_REG3_M_MD_POWERDOWN2
  		                               | LSM9DS1_CTRL_REG3_M_I2C_DISABLE, IMU_SPI_DEV_ID_M);
  imu_delay(10);
  /* Magnetometer +/-16gauss */
  imu_write_reg_dma(LSM9DS1_CTRL_REG2_M, LSM9DS1_CTRL_REG2_M_FS_16GAUSS, IMU_SPI_DEV_ID_M);
  /* enable Continuous-conversion mode, enable SPI 4-wire, disable I2C interface */
  imu_write_reg_dma(LSM9DS1_CTRL_REG3_M, LSM9DS1_CTRL_REG3_M_MD_CONT | LSM9DS1_CTRL_REG3_M_I2C_DISABLE, IMU_SPI_DEV_ID_M);
  /* Magnetometer 80Hz, Ultra-high performance mode(X/Y), Temperature compensation enable, Self-test enable */
  imu_write_reg_dma(LSM9DS1_CTRL_REG1_M, LSM9DS1_CTRL_REG1_M_DO_80HZ | LSM9DS1_CTRL_REG1_M_FAST_ODR
    		                               | LSM9DS1_CTRL_REG1_M_TEMP_COMP | LSM9DS1_CTRL_REG1_M_OM_ULTRAHIGH, IMU_SPI_DEV_ID_M);
  /* Magnetometer Ultra-high performance mode(Z) */
  imu_write_reg_dma(LSM9DS1_CTRL_REG4_M, LSM9DS1_CTRL_REG4_M_OMZ_ULTRAHIGH, IMU_SPI_DEV_ID_M);
  /* Magnetometer fast read */
  /* <! DO NOT DO THIS!!! >*/
//  imu_write_reg_dma(LSM9DS1_CTRL_REG5_M, LSM9DS1_CTRL_REG5_M_FAST_READ, IMU_SPI_DEV_ID_M);

#if FREERTOS_ENABLED
  imu_queue_create();
#endif /* FREERTOS_ENABLED */

  imu_configured = 1;
  /* read once to trig interrupt continuously */
  imu_read_reg_dma(LSM9DS1_OUT_X_L_G, 12, imu_rx_buffer, IMU_SPI_DEV_ID_G);
  imu_delay(1);
  imu_read_reg_dma(LSM9DS1_OUT_X_L_M | 0x40, 6, imu_mag_rx_buffer, IMU_SPI_DEV_ID_M);
  return 0;
}

void imu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit)
{
  unit->Acc.X = raw->Acc.X * 0.002392578125f;
  unit->Acc.Y = raw->Acc.Y * 0.002392578125f;
  unit->Acc.Z = raw->Acc.Z * 0.002392578125f;

  unit->Gyr.X = raw->Gyr.X * 0.06103515625f;
  unit->Gyr.Y = raw->Gyr.Y * 0.06103515625f;
  unit->Gyr.Z = raw->Gyr.Z * 0.06103515625f;

//  unit->Mag.X = raw->Mag.X * 0.0f;
//  unit->Mag.Y = raw->Mag.Y * 0.0f;
//  unit->Mag.Z = raw->Mag.Z * 0.0f;

  unit->TS = raw->TS;
}

void imu_int_2_callback(void) {
  uint32_t TS;
#if FREERTOS_ENABLED
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
  if(imu_configured == 1) {
    TS = _Get_Micros();
    imu_read_reg_dma(LSM9DS1_OUT_X_L_G, 12, imu_rx_buffer, IMU_SPI_DEV_ID_G);

    ((int8_t *)&imu_raw_data.Gyr.X)[0] = imu_rx_buffer[1];
    ((int8_t *)&imu_raw_data.Gyr.X)[1] = imu_rx_buffer[2];
    ((int8_t *)&imu_raw_data.Gyr.Y)[0] = imu_rx_buffer[3];
    ((int8_t *)&imu_raw_data.Gyr.Y)[1] = imu_rx_buffer[4];
    ((int8_t *)&imu_raw_data.Gyr.Z)[0] = imu_rx_buffer[5];
    ((int8_t *)&imu_raw_data.Gyr.Z)[1] = imu_rx_buffer[6];

    ((int8_t *)&imu_raw_data.Acc.X)[0] = imu_rx_buffer[7];
    ((int8_t *)&imu_raw_data.Acc.X)[1] = imu_rx_buffer[8];
    ((int8_t *)&imu_raw_data.Acc.Y)[0] = imu_rx_buffer[9];
    ((int8_t *)&imu_raw_data.Acc.Y)[1] = imu_rx_buffer[10];
    ((int8_t *)&imu_raw_data.Acc.Z)[0] = imu_rx_buffer[11];
    ((int8_t *)&imu_raw_data.Acc.Z)[1] = imu_rx_buffer[12];
    imu_raw_data.Acc.Y = -imu_raw_data.Acc.Y;
    imu_raw_data.TS = TS;

#if FREERTOS_ENABLED
    xQueueSendFromISR(imu_queue, &imu_raw_data, &xHigherPriorityTaskWoken);
#else

#endif /* FREERTOS_ENABLED */
  }
}

void imu_int_1_callback(void) {}
void imu_int_m_callback(void) {}
void imu_drdy_m_callback(void) {
  if(imu_configured == 1) {
    imu_read_reg_dma(LSM9DS1_OUT_X_L_M | 0x40, 6, imu_mag_rx_buffer, IMU_SPI_DEV_ID_M);
    ((int8_t *)&imu_raw_data.Mag.X)[0] = imu_mag_rx_buffer[1];
    ((int8_t *)&imu_raw_data.Mag.X)[1] = imu_mag_rx_buffer[2];
    ((int8_t *)&imu_raw_data.Mag.Y)[0] = imu_mag_rx_buffer[3];
    ((int8_t *)&imu_raw_data.Mag.Y)[1] = imu_mag_rx_buffer[4];
    ((int8_t *)&imu_raw_data.Mag.Z)[0] = imu_mag_rx_buffer[5];
    ((int8_t *)&imu_raw_data.Mag.Z)[1] = imu_mag_rx_buffer[6];
  }
}

#if FREERTOS_ENABLED
static void imu_queue_create(void)
{
  imu_queue = xQueueCreate(5, sizeof(IMU_RAW));
}

QueueHandle_t* imu_queue_get(void)
{
  return &imu_queue;
}
#endif /* FREERTOS_ENABLED */

/*
 * write data to the specific register.
 */
static void imu_write_reg_dma(uint8_t reg, uint8_t val, uint8_t id)
{
	imu_tx_buffer[0] = reg;
	imu_tx_buffer[1] = val;
	spi_rx_tx_dma_util(imu_tx_buffer, imu_rx_buffer, 2, id);
}

/*
 * read number of data from the specific register.
 */
static void imu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id)
{
	imu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma(imu_tx_buffer, r, num + 1, id);//ignore the first byte.
}

/*
 * read number of data from specific register until the operation completed.
 */
static void imu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id)
{
	imu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma_util(imu_tx_buffer, r, num + 1, id);//ignore the first byte.
}
