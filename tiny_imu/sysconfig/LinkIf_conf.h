/*
 * LinkIf_conf.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef LINKIF_CONF_H_
#define LINKIF_CONF_H_

#include "SysConfig.h"
#include "boardconfig.h"
#include "SysDataTypes.h"

#include "uart2.h"
#include "usbd_usr.h"
#include "usbd_cdc_vcp.h"

#define MAIN_DATA_CACHE                (32) /* 16 * n */
#define MSG_QUEUE_DEPTH                (3)

#define HARD_DEV_ID                    (0x5) /* for IMU Attitude Estimator board */

#define COM_IF_TX_CHECK()              (1)

#define COM_IF_TX_BYTES(p, l)          do { uart2_TxBytesDMA(p, l); if(USBD_isEnabled()) USB_CDC_SendBufferFast(p, l); } while(0)

#define COM_USER_TYPE                  TYPE_IMU_INFO_Resp = 0x11, \
                                       TYPE_ATT_QUAT_Resp = 0x12, \
									   TYPE_UPGRADE_REQUEST = 0x80

#define COM_USER_TYPE_DATA             IMU_INFO_DEF IMU_InfoData; \
                                       Quat_T AttitudeQuat; \
                                       FWInfoDef FileInfo;

#endif /* LINKIF_CONF_H_ */
