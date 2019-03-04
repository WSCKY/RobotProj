/*
 * LinkIf_conf.h
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#ifndef APPS_LINKIF_CONF_H_
#define APPS_LINKIF_CONF_H_

#include "uart.h"
#include "DataTypes.h"

#define HARD_DEV_ID                            (0x01) /* for upper monitor */

#define COM_IF_TX_CHECK()                      (uart_isopen())

#define COM_IF_TX_BYTES(p, l)                  uart_write((char *)p, (size_t)l)

#define COM_USER_TYPE                          TYPE_UPGRADE_REQUEST = 0x80, \
                                               TYPE_UPGRADE_DATA = 0x81, \
                                               TYPE_UPGRADE_FC_ACK = 0x82,

#define COM_USER_TYPE_DATA                     FWInfoDef FileInfo; \
                                               UpgradeDataDef PacketInfo; \
                                               DevResponseDef DevRespInfo;

#endif /* APPS_LINKIF_CONF_H_ */
