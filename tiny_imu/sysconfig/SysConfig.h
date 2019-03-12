/*
 * SysConfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include "stm32f0xx.h"

#include "boardconfig.h"
#include "SysDataTypes.h"

#include "TimerCounter.h"

#define FREERTOS_ENABLED               (0)
#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#define SYSTICK_ENABLE                 (0)

/* Interrupt Priority Table */
#define SYSTEM_TIMER_INT_PRIORITY      (0)

#define USB_DEVICE_INT_PRIORITY        (2)
#define IMU_SPI_DMA_INT_PRIORITY       (0)
#define IMU_UPDATE_INT_PRIORITY        (1)
#define UART2_RX_INT_PRIORITY          (0)
#define UART2_DMA_INT_PRIORITY         (0)
#if SYSTICK_ENABLE
#define SYSTICK_INT_PRIORITY           (3)
#endif /* SYSTICK_ENABLE */

#define UNUSED_VARIABLE(X)                       ((void)(X))
#define UNUSED_PARAMETER(X)                      UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X)                   UNUSED_VARIABLE(X)

void StartThread(void const * arg);

#endif /* SYSCONFIG_H_ */
