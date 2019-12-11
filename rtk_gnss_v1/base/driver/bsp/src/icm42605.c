/*
 * icm42605.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "icm42605.h"
#include <string.h>

static uint8_t imu_tx_buffer[15] = {0};
static uint8_t imu_rx_buffer[15] = {0};

#define IMU_DRV_CHECK_ASSERT(x) {if((x) != status_ok) return status_error;}

static status_t imu_read_reg(uint8_t reg, uint8_t num);
static status_t imu_write_reg(uint8_t reg, uint8_t val);

status_t icm42605_init(void)
{
  /* initialize spi to communicate to the icm42605 */
  IMU_DRV_CHECK_ASSERT(imuif_init());

  IMU_DRV_CHECK_ASSERT(imu_read_reg(REG_WHO_AM_I, 1));
  if(imu_rx_buffer[1] != IMU_ID) {
	  dbg_str("icm42605 read id failed.\n");
	  return status_error;
  } else {
	  dbg_str("icm42605 detected.\n");
  }

  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SOFT_RESET_ENABLE) );
  imu_delay(5);
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SPI_MODE_0_3 | IMU_SOFT_RESET_DISABLE) );

  memset(imu_tx_buffer, 0, 15);
  memset(imu_rx_buffer, 0, 15);
  return status_ok;
}

static status_t imu_read_reg(uint8_t reg, uint8_t num)
{
  if(num > 14) return status_error;
  imu_tx_buffer[0] = reg | 0x80;
  return imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, num +1);
}

static status_t imu_write_reg(uint8_t reg, uint8_t val)
{
  imu_tx_buffer[0] = reg;
  imu_tx_buffer[1] = val;
  return imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, 2);
}
