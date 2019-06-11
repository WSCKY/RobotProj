/*
 * SysDataTypes.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_SYSDATATYPES_H_
#define SYSCONFIG_SYSDATATYPES_H_

#include "kyLinkMacros.h"

__PACK_BEGIN typedef struct {
	int16_t accX, accY, accZ;
	int16_t temp;
	int16_t gyrX, gyrY, gyrZ;
	uint32_t TimeStamp;
} __PACK_END IMU_RAW;

__PACK_BEGIN typedef struct {
	int16_t accX, accY, accZ;
} __PACK_END AccRawDef;

__PACK_BEGIN typedef struct {
	int16_t gyrX, gyrY, gyrZ;
} __PACK_END GyrRawDef;

__PACK_BEGIN typedef struct {
	float accX, accY, accZ;
} __PACK_END AccDataDef;

__PACK_BEGIN typedef struct {
	float gyrX, gyrY, gyrZ;
} __PACK_END GyrDataDef;

typedef struct {
	AccDataDef AccData;
	float Temperature;
	GyrDataDef GyrData;
	uint32_t TimeStamp;
} __PACK_END IMU_UNIT;

__PACK_BEGIN typedef struct {
	float accX, accY, accZ;
	float gyrX, gyrY, gyrZ;
} __PACK_END IMU_INFO_DEF;

__PACK_BEGIN typedef struct {
	float qw, qx, qy, qz;
} __PACK_END Quat_T;

__PACK_BEGIN typedef struct {
	float pitch, roll, yaw;
} __PACK_END Euler_T;

__PACK_BEGIN typedef enum {
	FW_TYPE_NONE = 0,
	FW_TYPE_DRV_APP = 1,
	FW_TYPE_IMU_APP = 2,
	FW_TYPE_NAV_APP = 3,
	FW_TYPE_USC_APP = 4,
} __PACK_END FW_TYPE;

__PACK_BEGIN typedef enum {
	ENC_TYPE_PLAIN = 0,
	ENC_TYPE_AES_ECB = 1,
} __PACK_END FW_ENC_TYPE;

__PACK_BEGIN typedef struct {
	FW_TYPE FW_Type;
	FW_ENC_TYPE Enc_Type;
	uint32_t PacketNum;
	uint32_t FileSize;
	uint16_t FW_Version;
	uint32_t FileCRC;
} __PACK_END FWInfoDef;

__PACK_BEGIN typedef struct {
	float fData1, fData2, fData3, fData4, fData5, fData6, fData7, fData8, fData9;
} __PACK_END DebugFloatDef;

#endif /* SYSCONFIG_SYSDATATYPES_H_ */
