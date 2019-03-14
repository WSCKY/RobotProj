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

#define MSG_QUEUE_DEPTH                (3)

#define HARD_DEV_ID                    (0x5) /* for IMU Attitude Estimator board */

#define COM_IF_TX_CHECK()              (1)

#define COM_IF_TX_BYTES(p, l)          uart2_TxBytesDMA((uint8_t *)p, (uint32_t)l)

#define COM_USER_TYPE                  TYPE_UPGRADE_REQUEST = 0x80, \
                                       TYPE_UPGRADE_DATA = 0x81, \
                                       TYPE_UPGRADE_DEV_ACK = 0x82,

#define COM_USER_TYPE_DATA             FWInfoDef FileInfo; \
                                       UpgradeDataDef PacketInfo; \
                                       DevResponseDef DevRespInfo;

#endif /* LINKIF_CONF_H_ */
