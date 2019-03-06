/*
 * SysDataTypes.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_SYSDATATYPES_H_
#define SYSCONFIG_SYSDATATYPES_H_

#if !defined(__packed)
#define __packed __attribute__((packed))
#endif /* !defined(__packed) */

typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
	int16_t temp;
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
    uint32_t TimeStamp;
} __packed IMU_RAW;

typedef struct {
	int16_t accX;
	int16_t accY;
	int16_t accZ;
} __packed AccRawDef;

typedef struct {
	int16_t gyrX;
	int16_t gyrY;
	int16_t gyrZ;
} __packed GyrRawDef;

typedef struct {
	float accX;
	float accY;
	float accZ;
} __packed AccDataDef;

typedef struct {
	float gyrX;
	float gyrY;
	float gyrZ;
} __packed GyrDataDef;

typedef struct {
  AccDataDef AccData;
  float Temperature;
  GyrDataDef GyrData;
  uint32_t TimeStamp;
} __packed IMU_UNIT;

typedef struct {
  float accX;
	float accY;
	float accZ;
  float gyrX;
	float gyrY;
	float gyrZ;
} __packed IMU_INFO_DEF;

typedef struct {
  float qw;
  float qx;
  float qy;
  float qz;
} __packed Quat_T;

typedef struct {
  float pitch;
  float roll;
  float yaw;
} __packed Euler_T;

#endif /* SYSCONFIG_SYSDATATYPES_H_ */
