#ifndef __SYSCONFIG_H
#define __SYSCONFIG_H

#include "stm32f7xx_hal.h"

#include "SysDataTypes.h"

#include "cmsis_os.h"

#include "sys_def.h"

#include "log.h"

#include "boardconfig.h"

#define FREERTOS_ENABLED                         (1)

#if FREERTOS_ENABLED
#define kmm_alloc                                pvPortMalloc
#define kmm_free                                 vPortFree
#else
#include <stdlib.h>

#define kmm_alloc                                malloc
#define kmm_free                                 free
#endif /* FREERTOS_ENABLED */

/* Interrupt priority list */
#define INT_PRIORITY_MAX                         configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
#define INT_PRIORITY_MIN                         ((1 << __NVIC_PRIO_BITS) - 1)

#define INT_PRIORITY_LIMIT_MAX(prio)             ((prio) < INT_PRIORITY_MAX) ? INT_PRIORITY_MAX : (prio)
#define INT_PRIORITY_LIMIT_MIN(prio)             ((prio) > INT_PRIORITY_MIN) ? INT_PRIORITY_MIN : (prio)

#define INT_PRIORITY_LIMIT(prio)                 INT_PRIORITY_LIMIT_MIN(INT_PRIORITY_LIMIT_MAX(prio))

#define INT_PRIO_IMUIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)
#define INT_PRIO_IMUIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 0)

#define INT_PRIO_IMUIF_INT1                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 1)
#define INT_PRIO_IMUIF_INT2                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_GSMIF_PERIPH                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)
#define INT_PRIO_GSMIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 4)

#define INT_PRIO_UBXAIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)
#define INT_PRIO_UBXAIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)
#define INT_PRIO_UBXAIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 6)

#define INT_PRIO_UBXBIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)
#define INT_PRIO_UBXBIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)
#define INT_PRIO_UBXBIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)

#define INT_PRIO_MAGIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_MAGIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 8)
#define INT_PRIO_MAGIF_ER                        INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)
#define INT_PRIO_MAGIF_EV                        INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_COMIF_PERIPH                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_COMIF_DMARX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)
#define INT_PRIO_COMIF_DMATX                     INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 10)

#define INT_PRIO_OTGFS_INT                       INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 9)

#define INT_PRIO_FLASHIF_IT                      INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 2)

#define INT_PRIO_RTCMIF_PERIPH                   INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)
#define INT_PRIO_RTCMIF_DMARX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 5)
#define INT_PRIO_RTCMIF_DMATX                    INT_PRIORITY_LIMIT(INT_PRIORITY_MAX + 7)

#endif /* __SYSCONFIG_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
