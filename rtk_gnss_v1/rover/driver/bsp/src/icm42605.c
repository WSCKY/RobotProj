/*
 * icm42605.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "icm42605.h"
#include <string.h>

#define IMU_CACHE_SIZE                           17

static uint8_t imu_tx_buffer[IMU_CACHE_SIZE] = {0};
static uint8_t imu_rx_buffer[IMU_CACHE_SIZE] = {0};

#define IMU_DRV_CHECK_ASSERT(x) {if((x) != status_ok) return status_error;}

static status_t imu_read_reg(uint8_t reg, uint8_t num);
static status_t imu_write_reg(uint8_t reg, uint8_t val);

status_t icm42605_init(void)
{
  /* initialize spi to communicate to the icm42605 */
  IMU_DRV_CHECK_ASSERT(imuif_init());

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* software reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SOFT_RESET_ENABLE) );
  /* signal path reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_SIGNAL_PATH_RESET, \
                                      IMU_ABORT_AND_RESET | IMU_TMST_STROBE | IMU_FIFO_FLUSH) );
  imu_delay(50); // wait 50ms for reset operation done.
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  IMU_DRV_CHECK_ASSERT(imu_read_reg(REG_WHO_AM_I, 1));
  if(imu_rx_buffer[1] != IMU_ID) {
	  ky_err("icm42605 read id failed.\n");
	  return status_error;
  } else {
	  ky_err("icm42605 detected.\n");
  }

  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* AP interface uses 4-wire SPI mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG4, IMU_SPI_AP_4WIRE_USE_4WIRE) );

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
  /* UI data ready interrupt not routed to INT1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE0, IMU_UI_DRDY_INT1_EN) );
  /* UI data ready interrupt not routed to INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE3, IMU_UI_DRDY_INT2_EN) );
  /* configure watermark of FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG2, 1024 & 0xFF) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG3, (1024 >> 8) & IMU_FIFO_WM_UPPER_BITS_MASK) );
  /* FIFO hold last data, big-endian used */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG0, \
                                      IMU_FIFO_HOLD_LAST_DATA_EN | IMU_FIFO_COUNT_REC_RECORDS | IMU_FIFO_COUNT_ENDIAN_BIG | \
                                      IMU_SENSOR_DATA_ENDIAN_BIG | IMU_UI_SIFS_CFG_DISABLE_I2C) );
  /* Enable sensor data goes to FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG1, IMU_FIFO_RESUME_PARTIAL_RD | IMU_FIFO_WM_GT_TH | \
                                      IMU_FIFO_TMST_FSYNC_EN | IMU_FIFO_TEMP_EN | IMU_FIFO_GYRO_EN | IMU_FIFO_ACCEL_EN) );
  /* Tag FSYNC flag to TEMP_OUT LSB */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FSYNC_CONFIG, IMU_FSYNC_UI_SEL_TAG_TEMP_LSB | \
                                      IMU_FSYNC_UI_FLAG_CLEAR_SEL_UPDATE | IMU_FSYNC_POLARITY_RISING) );
  /* User should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_CONFIG1, IMU_INT_TPULSE_DURATION_100US | IMU_INT_TDEASSERT_DURATION_100US) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_TMST_CONFIG, 0x20 | IMU_TMST_TO_REGS_EN | IMU_TMST_RES_1US | \
                                      IMU_TMST_DELTA_EN  | IMU_TMST_FSYNC_EN | IMU_TMST_EN) );
  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* Enable gyroscope AAF & NF */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG_STATIC2, IMU_GYRO_AAF_ENABLE | IMU_GYRO_NF_ENABLE) );
  /* set PIN9 as INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG5, IMU_PIN9_FUNCTION_INT2) );
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* Clock selection */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG1, IMU_ACCEL_LP_CLK_SEL_WKUP_OSC | IMU_CLKSEL_PLL_OR_RC_OSC) );
  /* gyroscope: +/-2000dps@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG0, IMU_GYRO_FS_SEL_2000DPS | IMU_GYRO_ODR_200HZ) );
  /* accelerator: +/-16g@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG0, IMU_ACCEL_FS_SEL_8G | IMU_ACCEL_ODR_200HZ) );
  /* Bandwidth configuration */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_ACCEL_CONFIG0, (IMU_UI_FILT_BW_ODR_4 << IMU_UI_ACCEL_FILT_BW_BITSHIFT) |
                                                              (IMU_UI_FILT_BW_ODR_4 << IMU_UI_GYRO_FILT_BW_BITSHIFT)) );
  /* Selects order of ACCEL UI filter and Order of Accelerometer DEC2_M2 filter */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG1, IMU_ACCEL_UI_FILT_ORD_1ST | IMU_ACCEL_DEC2_M2_ORD_3RD) );
  /* temperature: DLPF@4000Hz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG1, IMU_TEMP_FILT_BW_4000HZ | IMU_GYRO_UI_FILT_ORD_1ST | IMU_GYRO_DEC2_M2_ORD_3RD) );
  /* Put gyroscope and accelerator sensor in low-noise mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_PWR_MGMT0, IMU_GYRO_MODE_LOW_NOISE | IMU_ACCEL_MODE_LOW_NOISE) );

  imu_delay(1);
//  /* select BANK2 */
//  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_2) );
//
//  IMU_DRV_CHECK_ASSERT( imu_write_reg(0x03, 0x7F) );

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  ky_info("icm42605 initialized.\n");

  memset(imu_tx_buffer, 0, IMU_CACHE_SIZE);
  memset(imu_rx_buffer, 0, IMU_CACHE_SIZE);
  return status_ok;
}
float imu_temp = 0.0f;
uint16_t imu_ts = 0;
int16_t imu_az = 0;
void icm42605_readfifo(void)
{
  imu_read_reg(REG_FIFO_DATA, 16);
//  imu_read_reg(REG_TEMP_DATA1, 16);
  imu_az = ((int16_t)imu_rx_buffer[6] << 8) | imu_rx_buffer[7];
  imu_temp = (imu_rx_buffer[14] / 2.07f) + 25;
  imu_ts = (((uint16_t)imu_rx_buffer[15] << 8) | imu_rx_buffer[16]);
//  imu_temp = (((uint16_t)imu_rx_buffer[1] << 8) | imu_rx_buffer[2]) / 132.48 + 25;
//  imu_ts = (((uint16_t)imu_rx_buffer[15] << 8) | imu_rx_buffer[16]);
}

static status_t imu_read_reg(uint8_t reg, uint8_t num)
{
  if(num > IMU_CACHE_SIZE - 1) return status_error;
  imu_tx_buffer[0] = reg | 0x80;
  return imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, num +1);
}

static status_t imu_write_reg(uint8_t reg, uint8_t val)
{
  imu_tx_buffer[0] = reg;
  imu_tx_buffer[1] = val;
  return imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, 2);
}
