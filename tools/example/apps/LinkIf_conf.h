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

#define HARD_DEV_ID                            (0x01) /* for upper monitor */

/* You can specify payload cache size here */
#define KYLINK_PAYLOAD_SIZE                    80

/* Your message id */
#define KYLINK_USER_TYPE                       TYPE_MY_DATA_1 = 0x12, \
									           TYPE_MY_DATA_2 = 0x34

/* Your package structure */
#define KYLINK_USER_TYPE_DATA                  uint16_t MY_SHORT_DATA; \
                                               TEST_DATA_T MY_STRUCT_DATA;

#endif /* APPS_LINKIF_CONF_H_ */
