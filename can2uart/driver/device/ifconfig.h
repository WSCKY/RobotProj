/*
 * board_config.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "SysConfig.h"

#include "uart2.h"

#if PRINT_LOG_ENABLE
#define LOG_PORT_PUT_CHAR                   uart2_TxByte
#endif

#endif /* BOARD_CONFIG_H_ */
