/*
 * att_est_q.c
 *
 *  Created on: Dec 13, 2019
 *      Author: kychu
 */

#include "att_est_q.h"
#include "quat_est.h"

#include "cpu_utils.h"

//static const char *TAG = "SINS";

static Euler_T est_e = {0, 0, 0};
static Quat_T est_q = {1, 0, 0, 0};

static IMU_RAW_6DOF imu_raw;
static IMU_UNIT_6DOF imu_unit;

//static _3AxisUnit gyr_off;
static uint32_t gyr_peace_flag = 0;

static void imu_peace_check(_3AxisUnit *pgyr);

void att_est_q_task(void const *argument)
{
  (void) argument;
  float delta_t = 1e-3;
  uint32_t last_t_us = 0, delta_t_us = 0;

  uint32_t tn, ts = 0;

  ky_info("att est task start.\n");
  osDelay(100);
  if(icm42605_init() != status_ok) {
	  ky_err("imu sensor init failed. EXIT!\n");
	  vTaskDelete(NULL);
	  return;
  }

  for(;;) {
    /* read IMU data form sensor */
    if(icm42605_read(&imu_raw, &imu_unit, osWaitForever) ==status_ok) {
      /* compute delta time according to time-stamp */
      if(imu_raw.TS > last_t_us)
        delta_t_us = imu_raw.TS - last_t_us;
      else
        delta_t_us = 0x10000 - last_t_us + imu_raw.TS;
      last_t_us = imu_raw.TS;
      delta_t = delta_t_us * 1e-6;

      imu_peace_check(&imu_unit.Gyr);

      fusionQ_6dot(&imu_unit, &est_q, 5, 0, delta_t);
      Quat2Euler(&est_q, &est_e);

      /* TEST CODE */
      tn = xTaskGetTickCountFromISR();
      if((tn - ts) > 1000) {
        ts = tn;
        ky_info("pitch: %2.2f, roll: %2.2f, yaw: %2.2f -%d-  [%2d%%]\n", est_e.pitch, est_e.roll, est_e.yaw, gyr_peace_flag, osGetCPUUsage());
      }
    }
  }
}

#define GYR_PEACE_THRESHOLD                      (0.5f) /* unit: dps */

static _3AxisUnit last_gyr = {0};
//static _3AxisUnit test_off_gyr = {10, 10, 10};
static void imu_peace_check(_3AxisUnit *pgyr)
{
  static uint32_t peace_ts = 0;
  static uint32_t peace_tn = 0;

  peace_tn = xTaskGetTickCount();
  if((fabs(pgyr->X - last_gyr.X) < GYR_PEACE_THRESHOLD) && \
     (fabs(pgyr->Y - last_gyr.Y) < GYR_PEACE_THRESHOLD) && \
     (fabs(pgyr->Z - last_gyr.Z) < GYR_PEACE_THRESHOLD)) {
    if(peace_tn - peace_ts > configTICK_RATE_HZ) { // keep 1s.
      if(gyr_peace_flag == 0) {
        gyr_peace_flag = 1;

//        gyr_off.X = pgyr->X;
//        gyr_off.Y = pgyr->Y;
//        gyr_off.Z = pgyr->Z;
//        icm42605_gyr_offset(&test_off_gyr);
        icm42605_gyr_offset(pgyr);
      }
    }
  } else {
    gyr_peace_flag = 0;
    peace_ts = peace_tn;
  }

  /* update the last data */
  last_gyr.X = pgyr->X;
  last_gyr.Y = pgyr->Y;
  last_gyr.Z = pgyr->Z;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
