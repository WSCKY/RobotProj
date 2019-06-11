/*
 * AttEst.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "AttEst.h"

#if MPU_DATA_UPDATE_HOOK_ENABLE

static Quat_T AttQ = {1, 0, 0, 0};
//static Euler_T AttE = {0, 0, 0};
static float fusionDt = 0.001f;
static float prop_gain_kp = 3.0f, intg_gain_ki = 0.0f;

static GyrRawDef GyrOffset;
static uint8_t gyr_calib_flag = 0; /* imu calibrated flag */
static void calib_loop(IMU_RAW *raw);

IMU_UNIT imu_unit;
uint32_t lastTimeStamp = 0;
void mpu_update_hook(IMU_RAW *pRaw)
{
	calib_loop(pRaw);
	if(gyr_calib_flag != 0) {
		pRaw->gyrX -= GyrOffset.gyrX;
		pRaw->gyrY -= GyrOffset.gyrY;
		pRaw->gyrZ -= GyrOffset.gyrZ;
		mpu_raw2unit(pRaw, &imu_unit);
		if(lastTimeStamp == 0) {
			lastTimeStamp = imu_unit.TimeStamp;
			fusionDt = 0.001f;
		} else {
			fusionDt = (float)(imu_unit.TimeStamp - lastTimeStamp) / 1000000.0f;
			if(fusionDt > 0.005f) {
				fusionDt = 0.001f;
			}
			lastTimeStamp = imu_unit.TimeStamp;
		}
		fusionQ_6dot(&imu_unit, &AttQ, prop_gain_kp, intg_gain_ki, fusionDt);
//		Quat2Euler(&AttQ, &AttE);
	}
}

Quat_T get_est_q(void)
{
	return AttQ;
}

IMU_UNIT get_imu_unit(void)
{
	return imu_unit;
}

#define CALIB_BUFF_SIZE                          (100)
static GyrRawDef GyrPeaceBuf[CALIB_BUFF_SIZE] = {0};
static uint8_t PeaceDataCnt = 0, PeaceDataIndex = 0;

static void calib_loop(IMU_RAW *raw)
{
  static uint32_t TimeStart = 0;
  static uint32_t TimeCurrent = 0, TimeStampDiv = 0;
  static int16_t gyrX = 0, gyrY = 0, gyrZ = 0;

  TimeCurrent = _Get_Millis();
  if(TimeCurrent - TimeStampDiv < 10) return; /* calib task divider. */
  TimeStampDiv = TimeCurrent;

  uint16_t turbulence = ABS(raw->gyrX - gyrX) + ABS(raw->gyrY - gyrY) + ABS(raw->gyrZ - gyrZ);
  gyrX = raw->gyrX; gyrY = raw->gyrY; gyrZ = raw->gyrZ;
  if(TimeStart == 0) {
    TimeStart = TimeCurrent; // init time stamp.
  }
  if((turbulence < 10) && ((ABS(raw->gyrX) < 80) && (ABS(raw->gyrY) < 80) && (ABS(raw->gyrZ) < 80))) {
//  if(turbulence < 15) {
    if((TimeCurrent - TimeStart) > 2000) { // keep 1 second.
      if(gyr_calib_flag == 0) {
        gyr_calib_flag = 1;
        GyrOffset.gyrX = raw->gyrX;
        GyrOffset.gyrY = raw->gyrY;
        GyrOffset.gyrZ = raw->gyrZ;
      } else {
        GyrPeaceBuf[PeaceDataIndex].gyrX = raw->gyrX;
        GyrPeaceBuf[PeaceDataIndex].gyrY = raw->gyrY;
        GyrPeaceBuf[PeaceDataIndex].gyrZ = raw->gyrZ;
        PeaceDataIndex ++;
        if(PeaceDataIndex >= CALIB_BUFF_SIZE)
          PeaceDataIndex = 0;
        if(PeaceDataCnt < CALIB_BUFF_SIZE) // fill the buffer first.
          PeaceDataCnt ++;
        else {
          GyrOffset.gyrX = GyrPeaceBuf[PeaceDataIndex].gyrX;
          GyrOffset.gyrY = GyrPeaceBuf[PeaceDataIndex].gyrY;
          GyrOffset.gyrZ = GyrPeaceBuf[PeaceDataIndex].gyrZ;
        }
      }
    }
  } else {
    TimeStart = TimeCurrent;
  }
}

#else
#error "MPU_DATA_UPDATE_HOOK_ENABLE UNSET"
#endif /* MPU_DATA_UPDATE_HOOK_ENABLE */
