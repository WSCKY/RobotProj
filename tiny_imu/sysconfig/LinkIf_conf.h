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

#define MAIN_DATA_CACHE                (32) /* 16 * n */
#define MSG_QUEUE_DEPTH                (3)

#define HARD_DEV_ID                    (0x5) /* for IMU Attitude Estimator board */

#define COM_IF_TX_CHECK()              (1)

#define COM_IF_TX_BYTES(p, l)          uart2_TxBytesDMA((uint8_t *)p, (uint32_t)l)

#define COM_USER_TYPE                  TYPE_IMU_INFO_Resp = 0x11, \
                                       TYPE_ATT_QUAT_Resp = 0x12,

#define COM_USER_TYPE_DATA             IMU_INFO_DEF IMU_InfoData; \
                                       Quat_T AttitudeQuat;

#endif /* LINKIF_CONF_H_ */
