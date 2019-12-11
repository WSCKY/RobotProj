#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stm32f7xx_hal.h"

#include "SysDataTypes.h"

#include "cmsis_os.h"

#include "sys_def.h"

#include "boardconfig.h"

#define DEBUG_ENABLE                             (1)

#define FREERTOS_ENABLE                          (1)

/* Interrupt priority list */
#define INT_PRIORITY_MAX                         configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define INT_PRIORITY_MIN                         ((1 << __NVIC_PRIO_BITS) - 1)

#define INT_PRIORITY_LIMIT_MAX(prio)             ((prio) < INT_PRIORITY_MAX) ? INT_PRIORITY_MAX : (prio)
#define INT_PRIORITY_LIMIT_MIN(prio)             ((prio) > INT_PRIORITY_MIN) ? INT_PRIORITY_MIN : (prio)

#define INT_PRIORITY_LIMIT(prio)                 INT_PRIORITY_LIMIT_MIN(INT_PRIORITY_LIMIT_MAX(prio))

#define INT_PRIO_IMUIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)
#define INT_PRIO_IMUIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)

#define INT_PRIO_GSMIF_PERIPH                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)

#define INT_PRIO_UBXAIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)
#define INT_PRIO_UBXAIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)
#define INT_PRIO_UBXAIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_UBXBIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_UBXBIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_UBXBIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)

#define INT_PRIO_COMIF_PERIPH                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_COMIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_COMIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)

#define INT_PRIO_RTCMIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)
#define INT_PRIO_RTCMIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)
#define INT_PRIO_RTCMIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)

#endif /* __SYSCONFIG_H */
