/*
 * SysConfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_

#include "stm32f0xx.h"

#include "SysDataTypes.h"

#include "TimerCounter.h"

#define FREERTOS_ENABLED               (0)
#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

#define UNUSED_VARIABLE(X)                       ((void)(X))
#define UNUSED_PARAMETER(X)                      UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X)                   UNUSED_VARIABLE(X)

void StartThread(void const * arg);

#endif /* SYSCONFIG_H_ */
