/*
 * LinkIf_conf.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef LINKIF_CONF_H_
#define LINKIF_CONF_H_

#include "SysConfig.h"
#include "SysDataTypes.h"

#define HARD_DEV_ID                    (0xC8) /* for CAN2UART board */

#define KYLINK_USER_TYPE               TYPE_CAN_DATA_Report = 0xC8,

#define KYLINK_USER_TYPE_DATA          CAN_NODE_MSG_DEF NodeData;

#endif /* LINKIF_CONF_H_ */
