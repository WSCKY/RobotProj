#ifndef __LINKIF_CONF_H
#define __LINKIF_CONF_H

#include "SysDataTypes.h"

#define HARD_DEV_ID                    (0x02) /* for NavBoard */

#define KYLINK_USER_TYPE               TYPE_IMU_INFO_Resp = 0x11, \
                                       TYPE_ATT_QUAT_Resp = 0x12,

#define KYLINK_USER_TYPE_DATA          IMU_INFO_DEF IMU_InfoData; \
                                       Quat_T AttitudeQuat;

#endif /* __LINKIF_CONF_H */