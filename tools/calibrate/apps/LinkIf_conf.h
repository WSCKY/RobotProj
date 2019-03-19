/*
 * LinkIf_conf.h
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#ifndef APPS_LINKIF_CONF_H_
#define APPS_LINKIF_CONF_H_

#include "SysConfig.h"

#include "uart.h"
#include "DataTypes.h"
#include "printPacket.h"

#define HARD_DEV_ID                            (0x01) /* for upper monitor */

#define KYLINK_USER_TYPE                       TYPE_IMU_INFO_Resp = 0x11, \
                                               TYPE_ATT_QUAT_Resp = 0x12, \
                                               TYPE_UPGRADE_REQUEST = 0x80, \
                                               TYPE_UPGRADE_DATA = 0x81, \
                                               TYPE_UPGRADE_DEV_ACK = 0x82, \
											   TYPE_UPPER_CALIB_REQ = 0xC0, \
									           TYPE_IMU_ORG_INFO_RESP = 0xC1

#define KYLINK_USER_TYPE_DATA                  IMU_INFO_DEF IMU_InfoData; \
                                               Quat_T AttitudeQuat; \
                                               FWInfoDef FileInfo; \
                                               UpgradeDataDef PacketInfo; \
                                               DevResponseDef DevRespInfo; \
                                               UpperCalibCmdDef CalibCmd; \
                                               IMU_RAW IMU_ORG_Data;

#endif /* APPS_LINKIF_CONF_H_ */
