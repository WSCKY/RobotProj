/*
 * lsm9ds1.c
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#include "lsm9ds1.h"

static uint8_t imu_configured = 0;

#if FREERTOS_ENABLED
static QueueHandle_t imu_queue = NULL;
#endif /* FREERTOS_ENABLED */

static uint8_t mpu_tx_buffer[15] = {0};
static uint8_t mpu_rx_buffer[15] = {0};

static void imu_write_reg_dma(uint8_t reg, uint8_t val, uint8_t id);
//static void imu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id);
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

  /* +/-8g @952Hz */
  imu_write_reg_dma(LSM9DS1_CTRL_REG6_XL, LSM9DS1_CTRL_REG6_XL_FS_XL_8G | LSM9DS1_CTRL_REG6_XL_ODR_XL_952HZ, IMU_SPI_DEV_ID_A);
  /* +/-2000dps @952Hz */
  imu_write_reg_dma(LSM9DS1_CTRL_REG1_G, LSM9DS1_CTRL_REG1_G_FS_G_2000DPS | LSM9DS1_CTRL_REG1_G_ODR_G_952HZ, IMU_SPI_DEV_ID_G);

#if FREERTOS_ENABLED
  imu_queue_create();
#endif /* FREERTOS_ENABLED */

  imu_configured = 1;
  return 0;
}

void imu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit)
{
  unit->AccData.accX = raw->accX * 0.002392578125f;
  unit->AccData.accY = raw->accY * 0.002392578125f;
  unit->AccData.accZ = raw->accZ * 0.002392578125f;
//  unit->Temperature = 21.0f + raw->temp / 338.87f;
  unit->GyrData.gyrX = raw->gyrX * 0.06103515625f;
  unit->GyrData.gyrY = raw->gyrY * 0.06103515625f;
  unit->GyrData.gyrZ = raw->gyrZ * 0.06103515625f;
  unit->TimeStamp = raw->TimeStamp;
}

void imu_int_1_callback(void) {}
void imu_int_2_callback(void) {}
void imu_int_m_callback(void) {}

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
	mpu_tx_buffer[0] = reg;
	mpu_tx_buffer[1] = val;
	spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2, id);
}

/*
 * read number of data from the specific register.
 */
//static void imu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id)
//{
//	mpu_tx_buffer[0] = reg | 0x80;
//	spi_rx_tx_dma(mpu_tx_buffer, r, num + 1, id);//ignore the first byte.
//}

/*
 * read number of data from specific register until the operation completed.
 */
static void imu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r, uint8_t id)
{
	mpu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma_util(mpu_tx_buffer, r, num + 1, id);//ignore the first byte.
}
