#ifndef __SYSDATATYPES_H
#define __SYSDATATYPES_H

#include "SysConfig.h"

#include "kyLinkMacros.h"

typedef void (*PortRecvByteCallback)(uint8_t Data);
typedef void (*PortRecvBytesCallback)(uint8_t *p, uint32_t l);

__PACK_BEGIN typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t temp;
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
	int16_t magX;
	int16_t magY;
	int16_t magZ;
  uint32_t TimeStamp;
} __PACK_END IMU_RAW;

__PACK_BEGIN typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
} __PACK_END AccRawDef;

__PACK_BEGIN typedef struct {
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
} __PACK_END GyrRawDef;

__PACK_BEGIN typedef struct {
	float accX;
	float accY;
	float accZ;
} __PACK_END AccDataDef;

__PACK_BEGIN typedef struct {
	float gyrX;
	float gyrY;
	float gyrZ;
} __PACK_END GyrDataDef;

__PACK_BEGIN typedef struct {
	float magX;
	float magY;
	float magZ;
} __PACK_END MagDataDef;

__PACK_BEGIN typedef struct {
  AccDataDef AccData;
  float Temperature;
  GyrDataDef GyrData;
  MagDataDef MagData;
  uint32_t TimeStamp;
} __PACK_END IMU_UNIT;

__PACK_BEGIN typedef struct {
  float accX;
	float accY;
	float accZ;
  float gyrX;
	float gyrY;
	float gyrZ;
} __PACK_END IMU_INFO_DEF;

__PACK_BEGIN typedef struct {
  float accX;
  float accY;
  float accZ;
  float gyrX;
  float gyrY;
  float gyrZ;
  float magX;
  float magY;
  float magZ;
} __PACK_END IMU_9DOF_DEF;

__PACK_BEGIN typedef struct {
  float qw;
  float qx;
  float qy;
  float qz;
} __PACK_END Quat_T;

__PACK_BEGIN typedef struct {
  float pitch;
  float roll;
  float yaw;
} __PACK_END Euler_T;

#endif /* __SYSDATATYPES_H */
