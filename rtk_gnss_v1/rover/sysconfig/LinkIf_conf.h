#ifndef __LINKIF_CONF_H
#define __LINKIF_CONF_H

#include "SysDataTypes.h"
#include "f9pconfig.h"

#define HARD_DEV_ID                    (0x24) /* for RTK ROVER */

#if (FREERTOS_ENABLED)
#define KYLINK_TIMEOUT                 1000
#endif /* FREERTOS_ENABLED */

#endif /* __LINKIF_CONF_H */
