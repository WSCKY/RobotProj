/*
 * SysDataTypes.h
 *
 *  Created on: Mar 29, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_SYSDATATYPES_H_
#define SYSCONFIG_SYSDATATYPES_H_

#include "kyLinkMacros.h"

__PACK_BEGIN typedef struct {
	uint32_t NodeID;
	float NodeData1;
	float NodeData2;
} __PACK_END CAN_NODE_MSG_DEF;

#endif /* SYSCONFIG_SYSDATATYPES_H_ */
