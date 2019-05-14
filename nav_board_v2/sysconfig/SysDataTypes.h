#ifndef __SYSDATATYPES_H
#define __SYSDATATYPES_H

#include "SysConfig.h"

#include "kyLinkMacros.h"

typedef void (*PortRecvByteCallback)(uint8_t Data);
typedef void (*PortRecvBytesCallback)(uint8_t *p, uint32_t l);

__PACK_BEGIN typedef struct {
  int16_t X;
  int16_t Y;
  int16_t Z;
} __PACK_END _3AxisRaw;

__PACK_BEGIN typedef struct {
  _3AxisRaw Acc;
  _3AxisRaw Gyr;
  _3AxisRaw Mag;
  uint32_t TS;
} __PACK_END IMU_RAW;

__PACK_BEGIN typedef struct {
  float X;
  float Y;
  float Z;
} __PACK_END _3AxisUnit;

__PACK_BEGIN typedef struct {
  _3AxisUnit Acc;
  _3AxisUnit Gyr;
  _3AxisUnit Mag;
  uint32_t TS;
} __PACK_END IMU_UNIT;

__PACK_BEGIN typedef struct {
  float accX;
  float accY;
  float accZ;
  float gyrX;
  float gyrY;
  float gyrZ;
  uint32_t TS;
} __PACK_END IMU_6DOF_DEF;

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
  uint32_t TS;
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
