#ifndef __MPU9250_H
#define __MPU9250_H

#include "imu_spi.h"

#if FREERTOS_ENABLED
#define mpu_delay                      osDelay
#else
#include "TimerCounter.h"
#define mpu_delay                      _delay_ms
#endif

void mpu9250_init(void);
#if FREERTOS_ENABLED
QueueHandle_t* mpu_queue_get(void);
#endif /* FREERTOS_ENABLED */
void mpu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit);
void mpu_set_gyr_off(int16_t x, int16_t y, int16_t z);

#endif /* __MPU9250_H */
