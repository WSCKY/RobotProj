#ifndef __LINKIF_CONF_H
#define __LINKIF_CONF_H

#include "SysDataTypes.h"
#include "f9pconfig.h"

#define HARD_DEV_ID                    (0x24) /* for RTK ROVER */

#define KYLINK_USER_TYPE               TYPE_RTK_Info_Resp = 0x61, \
                                       TYPE_PVTS_Info_Resp = 0x56

#define KYLINK_USER_TYPE_DATA          Location_T LocationInfo; \
                                       ubx_npvts_t pvts;

#endif /* __LINKIF_CONF_H */
