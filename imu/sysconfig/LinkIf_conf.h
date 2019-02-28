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

#define MAIN_DATA_CACHE                (32) /* 16 * n */
#define MSG_QUEUE_DEPTH                (3)

#define HARD_DEV_ID                    (0x5) /* for IMU Attitude Estimator board */

#define COM_USER_TYPE

#define COM_USER_TYPE_DATA

#endif /* LINKIF_CONF_H_ */
