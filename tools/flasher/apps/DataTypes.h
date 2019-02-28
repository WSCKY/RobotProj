/*
 * DataTypes.h
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#ifndef APPS_DATATYPES_H_
#define APPS_DATATYPES_H_

#include "ComTypes.h"

#define FILE_DATA_CACHE                        (80) /* 16 * n */
#define MAIN_DATA_CACHE                        (FILE_DATA_CACHE + 5)

#define __packed __attribute__((packed))

typedef enum {
	FW_TYPE_NONE = 0,
	FW_TYPE_APP = 1,
} __packed FW_TYPE;

typedef enum {
	IsReady = 0,
	InErasing = 1,
	Upgrading = 2,
	Refused = 3,
	JumpFailed = 4,
} __packed DevpgradeState;

typedef struct {
	FW_TYPE FW_Type;
	uint32_t PacketNum;
	uint32_t FileSize;
	uint16_t FW_Version;
	uint8_t DroneType;
	uint32_t FileCRC;
} __packed FWInfoDef;

typedef struct {
	uint32_t PacketID;
	uint8_t PacketLen;
	uint8_t PacketData[FILE_DATA_CACHE];
} __packed UpgradeDataDef;

typedef struct {
	DevpgradeState FC_State;
	uint8_t reserve[5];
} __packed DevResponseDef;

#endif /* APPS_DATATYPES_H_ */
