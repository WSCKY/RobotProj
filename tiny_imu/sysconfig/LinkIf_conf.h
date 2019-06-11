/*
 * LinkIf_conf.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef LINKIF_CONF_H_
#define LINKIF_CONF_H_

#include "SysDataTypes.h"
#include "boardconfig.h"

#define HARD_DEV_ID                    (0x5) /* for IMU Attitude Estimator board */

#define KYLINK_USER_TYPE               TYPE_IMU_INFO_Resp = 0x11, \
                                       TYPE_ATT_QUAT_Resp = 0x12, \
									   TYPE_UPGRADE_REQUEST = 0x80, \
									   TYPE_DEBUG_DATA_Resp = 0xDC,

#define KYLINK_USER_TYPE_DATA          IMU_INFO_DEF IMU_InfoData; \
                                       Quat_T AttitudeQuat; \
                                       FWInfoDef FileInfo; \
                                       DebugFloatDef DebugData;

#endif /* LINKIF_CONF_H_ */
