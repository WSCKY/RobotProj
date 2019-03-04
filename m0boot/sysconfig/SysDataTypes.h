/*
 * SysDataTypes.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_SYSDATATYPES_H_
#define SYSCONFIG_SYSDATATYPES_H_

#include "ComTypes.h"

#if !defined(__packed)
#define __packed __attribute__((packed))
#endif /* !defined(__packed) */

#define FILE_DATA_CACHE                        (80) /* 16 * n */
#define MAIN_DATA_CACHE                        (FILE_DATA_CACHE + 5)

typedef enum {
	FW_TYPE_NONE = 0,
	FW_TYPE_DRV_APP = 1,
	FW_TYPE_IMU_APP = 2,
	FW_TYPE_NAV_APP = 3,
	FW_TYPE_USC_APP = 4,
} __packed FW_TYPE;

typedef enum {
	ENC_TYPE_PLAIN = 0,
	ENC_TYPE_AES_ECB = 1,
} __packed FW_ENC_TYPE;

typedef enum {
	IsReady = 0,
	InErasing = 1,
	Upgrading = 2,
	Refused = 3,
	JumpFailed = 4,
} __packed DevpgradeState;

typedef struct {
	FW_TYPE FW_Type;
	FW_ENC_TYPE Enc_Type;
	uint32_t PacketNum;
	uint32_t FileSize;
	uint16_t FW_Version;
	uint32_t FileCRC;
} __packed FWInfoDef;

typedef struct {
	uint32_t PacketID;
	uint8_t PacketLen;
	uint8_t PacketData[FILE_DATA_CACHE];
} __packed UpgradeDataDef;

typedef struct {
	DevpgradeState Dev_State;
	uint8_t reserve[5];
} __packed DevResponseDef;

#endif /* SYSCONFIG_SYSDATATYPES_H_ */
