#ifndef __LINKIF_CONF_H
#define __LINKIF_CONF_H

#include "SysDataTypes.h"

#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"

#define MAIN_DATA_CACHE                (32) /* 16 * n */
#define MSG_QUEUE_DEPTH                (3)

#define HARD_DEV_ID                    (0x02) /* for NavBoard */

#define COM_IF_TX_BYTES(p, l)          USB_CDC_SendBufferFast(p, l)
#define COM_IF_TX_CHECK()              USBD_isEnabled()

#define COM_USER_TYPE                  TYPE_IMU_INFO_Resp = 0x11, \
                                       TYPE_ATT_QUAT_Resp = 0x12,

#define COM_USER_TYPE_DATA             IMU_INFO_DEF IMU_InfoData; \
                                       Quat_T AttitudeQuat;

#endif /* __LINKIF_CONF_H */
