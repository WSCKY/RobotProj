/*
 * lsm9ds1.h
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#ifndef DRIVER_BSP_INC_LSM9DS1_H_
#define DRIVER_BSP_INC_LSM9DS1_H_

#include "imu_spi.h"
#include "lsm9ds1_reg.h"

#if FREERTOS_ENABLED
#define imu_delay                      osDelay
#else
#include "TimerCounter.h"
#define imu_delay                      _delay_ms
#endif

int lsm9ds1_init(void);
QueueHandle_t* imu_queue_get(void);
void imu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit);

#endif /* DRIVER_BSP_INC_LSM9DS1_H_ */
