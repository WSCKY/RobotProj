#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#include "boardconfig.h"

#include "cmsis_os.h"
#include "SysDataTypes.h"
#include "TaskPriorityConfig.h"
#include "PeriphPriorityConfig.h"
/* System Timer clock */
#include "TimerCounter.h"

#define FREERTOS_ENABLED                         (1)

#define UNUSED_VARIABLE(X)                       ((void)(X))
#define UNUSED_PARAMETER(X)                      UNUSED_VARIABLE(X)
#define UNUSED_RETURN_VALUE(X)                   UNUSED_VARIABLE(X)

#endif /* __SYSCONFIG_H */
