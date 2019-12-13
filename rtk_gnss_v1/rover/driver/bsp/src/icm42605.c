/*
 * icm42605.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "icm42605.h"
#include <string.h>

#define IMU_CACHE_SIZE                           17

static uint32_t icm42605_init_flag = 0;
#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
static osSemaphoreId imu_semaphore;
static uint8_t *imu_tx_buffer = NULL;
static uint8_t *imu_rx_buffer = NULL;
static IMU_RAW_6DOF *imu_raw_data = NULL;
#else
static uint32_t imu_data_ready = 0;
static IMU_RAW_6DOF imu_raw_data = {0};
static uint8_t imu_tx_buffer[IMU_CACHE_SIZE] = {0};
static uint8_t imu_rx_buffer[IMU_CACHE_SIZE] = {0};
#endif /* FREERTOS_ENABLED */

#define IMU_DRV_CHECK_ASSERT(x) {if((x) != status_ok) return status_error;}

static status_t imu_read_reg(uint8_t reg, uint8_t num);
static status_t imu_write_reg(uint8_t reg, uint8_t val);

status_t icm42605_init(void)
{
  if(icm42605_init_flag == 1) return status_ok;

#if FREERTOS_ENABLED
  imu_tx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_tx_buffer == NULL) return status_nomem;
  imu_rx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_rx_buffer == NULL) return status_nomem;
  imu_raw_data = kmm_alloc(sizeof(IMU_RAW_6DOF)); if(imu_raw_data == NULL) return status_nomem;

  /* Create the mutex  */
  osMutexDef(IMUIFMutex);
  if_mutex = osMutexCreate(osMutex(IMUIFMutex));
  if(if_mutex == NULL) return status_error;

  /* Define used semaphore */
  osSemaphoreDef(IMU_SEM);
  /* Create the semaphore */
  imu_semaphore = osSemaphoreCreate(osSemaphore(IMU_SEM) , 1);
  if(imu_semaphore == NULL) return status_error;
#else
  imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

  /* initialize spi to communicate to the icm42605 */
  IMU_DRV_CHECK_ASSERT(imuif_init());

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* software reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SOFT_RESET_ENABLE) );
  /* signal path reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_SIGNAL_PATH_RESET, \
                                      IMU_ABORT_AND_RESET | IMU_TMST_STROBE | IMU_FIFO_FLUSH) );
  imu_delay(100); // wait 50ms for reset operation done.
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  IMU_DRV_CHECK_ASSERT(imu_read_reg(REG_WHO_AM_I, 1));
  if(imu_rx_buffer[1] != IMU_ID) {
	  ky_err("icm42605 read id failed.\n");
	  return status_error;
  }

  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* AP interface uses 4-wire SPI mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG4, 0x01 | IMU_SPI_AP_4WIRE_USE_4WIRE) );

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* select spi mode 0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SPI_MODE_0_3 | IMU_SOFT_RESET_DISABLE) );
  imu_delay(1);
  /* FIFO mode: Stream-to-FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG, IMU_FIFO_MODE_STREAM) );
  /* INT1 & INT2 Pin configuration */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_CONFIG, \
                                     (IMU_INT_MODE_PULSED | IMU_INT_DRIVE_PP | IMU_INT_POLARITY_HIGH) << IMU_INT2_CFG_BITSHIFT | \
                                     (IMU_INT_MODE_PULSED | IMU_INT_DRIVE_PP | IMU_INT_POLARITY_HIGH) << IMU_INT1_CFG_BITSHIFT));
  /* UI data ready interrupt routed to INT1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE0, IMU_UI_DRDY_INT1_EN) );
  /* UI data ready interrupt routed to INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE3, IMU_UI_DRDY_INT2_EN) );
  /* configure watermark of FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG2, 1024 & 0xFF) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG3, (1024 >> 8) & IMU_FIFO_WM_UPPER_BITS_MASK) );
  /* FIFO hold last data, big-endian used */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG0, \
                                      IMU_FIFO_HOLD_LAST_DATA_EN | IMU_FIFO_COUNT_REC_RECORDS | IMU_FIFO_COUNT_ENDIAN_LITTLE | \
                                      IMU_SENSOR_DATA_ENDIAN_LITTLE | IMU_UI_SIFS_CFG_DISABLE_I2C) );
  /* Enable sensor data goes to FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG1, IMU_FIFO_RESUME_PARTIAL_RD | IMU_FIFO_WM_GT_TH | \
                                      IMU_FIFO_TMST_FSYNC_EN | IMU_FIFO_TEMP_EN | IMU_FIFO_GYRO_EN | IMU_FIFO_ACCEL_EN) );
  /* Tag FSYNC flag to TEMP_OUT LSB */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FSYNC_CONFIG, IMU_FSYNC_UI_SEL_TAG_TEMP_LSB | \
                                      IMU_FSYNC_UI_FLAG_CLEAR_SEL_UPDATE | IMU_FSYNC_POLARITY_RISING) );
  /* User should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_CONFIG1, IMU_INT_TPULSE_DURATION_100US | IMU_INT_TDEASSERT_DURATION_100US) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_TMST_CONFIG, 0x20 | IMU_TMST_TO_REGS_EN | IMU_TMST_RES_1US | \
                                      IMU_TMST_FSYNC_EN | IMU_TMST_EN) );
  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* Enable gyroscope AAF & NF */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG_STATIC2, 0xA8 | IMU_GYRO_AAF_ENABLE | IMU_GYRO_NF_ENABLE) );
  /* set PIN9 as INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG5, 0x20 | IMU_PIN9_FUNCTION_INT2) );
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* Clock selection */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG1, 0x90 | IMU_ACCEL_LP_CLK_SEL_WKUP_OSC | IMU_CLKSEL_PLL_OR_RC_OSC) );
  /* gyroscope: +/-2000dps@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG0, IMU_GYRO_FS_SEL_2000DPS | IMU_GYRO_ODR_1KHZ) );
  /* accelerator: +/-16g@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG0, IMU_ACCEL_FS_SEL_8G | IMU_ACCEL_ODR_1KHZ) );
  /* Bandwidth configuration */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_ACCEL_CONFIG0, (IMU_UI_FILT_BW_ODR_4 << IMU_UI_ACCEL_FILT_BW_BITSHIFT) |
                                                              (IMU_UI_FILT_BW_ODR_4 << IMU_UI_GYRO_FILT_BW_BITSHIFT)) );
  /* Selects order of ACCEL UI filter and Order of Accelerometer DEC2_M2 filter */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG1, 0x01 | IMU_ACCEL_UI_FILT_ORD_1ST | IMU_ACCEL_DEC2_M2_ORD_3RD) );
  /* temperature: DLPF@4000Hz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG1, 0x10 | IMU_TEMP_FILT_BW_4000HZ | IMU_GYRO_UI_FILT_ORD_1ST | IMU_GYRO_DEC2_M2_ORD_3RD) );
  /* Put gyroscope and accelerator sensor in low-noise mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_PWR_MGMT0, IMU_GYRO_MODE_LOW_NOISE | IMU_ACCEL_MODE_LOW_NOISE) );

  imu_delay(1);
//  /* select BANK2 */
//  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_2) );
//
//  IMU_DRV_CHECK_ASSERT( imu_write_reg(0x03, 0x7F) );

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  memset(imu_tx_buffer, 0, IMU_CACHE_SIZE);
  memset(imu_rx_buffer, 0, IMU_CACHE_SIZE);

  icm42605_init_flag = 1;
  ky_info("icm42605 initialized.\n");

  return status_ok;
}

status_t icm42605_read(IMU_RAW_6DOF *raw, IMU_UNIT_6DOF *unit, uint32_t timeout)
{
  IMU_RAW_6DOF *p;
#if FREERTOS_ENABLED
  p = imu_raw_data;

  if(osSemaphoreWait(imu_semaphore , timeout) == osOK) {
#else
  p = &imu_raw_data;

  uint32_t st = HAL_GetTick();
  do {
    if(timeout == 0) {
      break;
    } else if(timeout != HAL_MAX_DELAY) {
      if((HAL_GetTick() - st) > timeout)
        return status_timeout;
    }
  } while(imu_data_ready == 0);
  if(imu_data_ready != 0) {
    imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

    p->Acc.X = *((int16_t *)&(imu_rx_buffer[2]));
    p->Acc.Y = *((int16_t *)&(imu_rx_buffer[4]));
    p->Acc.Z = *((int16_t *)&(imu_rx_buffer[6]));

    p->Gyr.X = *((int16_t *)&(imu_rx_buffer[8]));
    p->Gyr.Y = *((int16_t *)&(imu_rx_buffer[10]));
    p->Gyr.Z = *((int16_t *)&(imu_rx_buffer[12]));

    p->Temp = (int8_t)imu_rx_buffer[14];
    p->TS = *((uint16_t *)&(imu_rx_buffer[15]));

    *raw = *p;

#if FREERTOS_ENABLED
    osMutexWait(if_mutex, osWaitForever);
#endif /* FREERTOS_ENABLED */
    imu_tx_buffer[0] = REG_FIFO_DATA | 0x80;
    imuif_txrx_bytes_dma(imu_tx_buffer, imu_rx_buffer, 16 +1);
#if FREERTOS_ENABLED
    osMutexRelease(if_mutex);
#endif /* FREERTOS_ENABLED */

    unit->Acc.X = p->Acc.X * 0.002392578125f;
    unit->Acc.Y = p->Acc.Y * 0.002392578125f;
    unit->Acc.Z = p->Acc.Z * 0.002392578125f;

    unit->Gyr.X = p->Gyr.X * 0.06103515625f;
    unit->Gyr.Y = p->Gyr.Y * 0.06103515625f;
    unit->Gyr.Z = p->Gyr.Z * 0.06103515625f;

    unit->Temp = p->Temp / 2.07f + 25.0f;
    unit->TS = p->TS;

    return status_ok;
  }

  return status_timeout;
}

void imuif_int1_callback(void)
{
  if(icm42605_init_flag != 0) {
#if FREERTOS_ENABLED
    osSemaphoreRelease(imu_semaphore);
#else
    imu_data_ready = 1;
#endif /* FREERTOS_ENABLED */
  }
}

void imuif_int2_callback(void)
{}

static status_t imu_read_reg(uint8_t reg, uint8_t num)
{
  status_t ret = status_error;
  if(num > IMU_CACHE_SIZE - 1) return ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#endif /* FREERTOS_ENABLED */
  imu_tx_buffer[0] = reg | 0x80;
  ret = imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, num +1);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#endif /* FREERTOS_ENABLED */
  return ret;
}

static status_t imu_write_reg(uint8_t reg, uint8_t val)
{
  status_t ret = status_error;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#endif /* FREERTOS_ENABLED */
  imu_tx_buffer[0] = reg;
  imu_tx_buffer[1] = val;
  ret = imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, 2);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#endif /* FREERTOS_ENABLED */
  return ret;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
