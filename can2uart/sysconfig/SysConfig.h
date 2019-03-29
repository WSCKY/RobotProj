/*
 * SysConfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include <stdint.h>
#include "stm32f0xx.h"

#include "boardconfig.h"
#include "TimerCounter.h"

#define MONITOR_TYPE_TERMINAL               (0)

#define PRINT_LOG_ENABLE                    (1)

#define USC_MODE_ECHO                       (0)
#define USC_MODE_UART                       (1)
#define USC_MODE                            USC_MODE_UART

#define FREERTOS_ENABLED               (0)
#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#define SYSTICK_ENABLE                 (0)

/* Interrupt Priority Table */
#define SYSTEM_TIMER_INT_PRIORITY      (0)

#define UART2_RX_INT_PRIORITY          (0)
#define UART2_DMA_INT_PRIORITY         (0)

#if SYSTICK_ENABLE
#define SYSTICK_INT_PRIORITY           (3)
#endif /* SYSTICK_ENABLE */

#define UNUSED_VARIABLE(X)             ((void)(X))
#define UNUSED_PARAMETER(X)            UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X)         UNUSED_VARIABLE(X)

void StartThread(void const * arg);

#endif /* SYSCONFIG_H_ */
