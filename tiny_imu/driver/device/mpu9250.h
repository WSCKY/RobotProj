/*
 * mpu9250.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef __MPU9250_H
#define __MPU9250_H

#include "ifconfig.h"

#if FREERTOS_ENABLED
#define mpu_delay                      osDelay
#else
#define mpu_delay                      _delay_ms
#endif

#if !FREERTOS_ENABLED
#define MPU_DATA_UPDATE_HOOK_ENABLE    (1)
#endif /* !FREERTOS_ENABLED */

#define imu_spi_init              spi1_init
#define imu_int_init              intio_init
#define imu_int_setcallback       intio_set_irq_handler
#define imu_spi_config_rate       spi1_configrate
#define spi_rx_tx_dma             spi1_rx_tx_dma
#define spi_rx_tx_dma_util        spi1_rx_tx_dma_util

#define IMU_SPI_SLOW_RATE         SPI_BaudRatePrescaler_128 /* 48 / 64 = 0.75MHz */
#define IMU_SPI_FAST_RATE         SPI_BaudRatePrescaler_8   /* 48 / 8 = 6MHz */

#define MPU_DATA_DEPTH            (1 << 1)
#define MPU_BUFF_MASK             (MPU_DATA_DEPTH - 1)

uint8_t mpu9250_init(void);
#if FREERTOS_ENABLED
QueueHandle_t* mpu_queue_get(void);
#else
#if MPU_DATA_UPDATE_HOOK_ENABLE
void mpu_update_hook(IMU_RAW *pRaw);
#else
uint8_t mpu_push_new(IMU_RAW *pRaw);
uint8_t mpu_pull_new(IMU_RAW *pRaw);
#endif /* MPU_DATA_UPDATE_HOOK_ENABLE */
#endif /* FREERTOS_ENABLED */
void mpu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit);
void mpu_set_gyr_off(int16_t x, int16_t y, int16_t z);

#endif /* __MPU9250_H */
