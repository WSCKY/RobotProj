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

#define FREERTOS_ENABLED               (1)
#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */

void StartThread(void const * argument);

#endif /* SYSCONFIG_H_ */
