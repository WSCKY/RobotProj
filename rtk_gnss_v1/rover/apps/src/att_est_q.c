/*
 * att_est_q.c
 *
 *  Created on: Dec 13, 2019
 *      Author: kychu
 */

#include "att_est_q.h"
#include "quat_est.h"

#include "mesg.h"
#include "kyLink.h"

#include "cpu_utils.h"

//static const char *TAG = "SINS";

static Euler_T est_e = {0, 0, 0};
static Quat_T est_q = {1, 0, 0, 0};

static IMU_RAW_6DOF imu_raw;
static IMU_UNIT_6DOF imu_unit;

static COM_MSG_DEF msg_quat;
static Quat_T msg_quat_store;
static msg_wr_state msg_quat_flag = msg_read;

//static _3AxisUnit gyr_off;
static uint32_t imu_selftest_done = 0;
static uint32_t gyr_peace_flag = 0;

static void imu_peace_check(_3AxisUnit *pgyr);

void att_est_q_task(void const *argument)
{
  (void) argument;
  uint8_t st_ret = 0;
  float delta_t = 1e-3;
  uint32_t last_t_us = 0, delta_t_us = 0;

  uint32_t time_now, time_start = 0;//, tcnt = 0;

  ky_info("att est task start.\n");
  osDelay(100);
  if(icm42605_init() != status_ok) {
	  ky_err("imu sensor init failed. EXIT!\n");
	  vTaskDelete(NULL);
	  return;
  }
  ky_info("keep IMU motionless ...\n");
  while(imu_selftest_done == 0) {
    /* read IMU data form sensor */
    if(icm42605_read(&imu_raw, &imu_unit, osWaitForever) == status_ok) {
      /* check for motionless state */
      imu_peace_check(&imu_unit.Gyr);
      if(gyr_peace_flag == 1) {
        if(icm42605_selftest(&imu_raw, &st_ret) != status_ok) {
          ky_err("imu self test faileds. EXIT!\n");
          vTaskDelete(NULL);
          return;
        }

        if(st_ret & 0x01)
          ky_info("gyr st PASS.\n");
        else
          ky_info("gyr st FAIL.\n");

        if(st_ret & 0x02)
          ky_info("acc st PASS.\n");
        else
          ky_info("acc st FAIL.\n");
        imu_selftest_done = 1;
      }
    }
  }

  for(;;) {
    /* read IMU data form sensor */
    if(icm42605_read(&imu_raw, &imu_unit, osWaitForever) == status_ok) {
      /* compute delta time according to time-stamp */
      if(imu_raw.TS > last_t_us)
        delta_t_us = imu_raw.TS - last_t_us;
      else
        delta_t_us = 0x10000 - last_t_us + imu_raw.TS;
      last_t_us = imu_raw.TS;
      delta_t = delta_t_us * 1e-6;

//      imu_peace_check(&imu_unit.Gyr);

      fusionQ_6dot(&imu_unit, &est_q, 5, 0, delta_t);
      Quat2Euler(&est_q, &est_e);

      /* TEST CODE */
      time_now = xTaskGetTickCountFromISR();
      if((time_now - time_start) > 10) {
        time_start = time_now;

        if(msg_quat_flag == msg_read) {
          msg_quat_flag = msg_write;
          msg_quat_store = est_q;
          msg_quat.len = sizeof(Quat_T);
          msg_quat.type = TYPE_QUAT_Info_Resp;
          msg_quat.state = &msg_quat_flag;
          msg_quat.pointer = &msg_quat_store;
          mesg_send_mesg(&msg_quat);
        }
//        tcnt ++;
//        led_on(LED_BLUE);
//        if(tcnt & 1)
//          ky_info("g:%d, %d, %d\na:%d, %d, %d\n", imu_raw.Gyr.X, imu_raw.Gyr.Y, imu_raw.Gyr.Z
//                                                , imu_raw.Acc.X, imu_raw.Acc.Y, imu_raw.Acc.Z);
//        	ky_info("g:%2.2f, %2.2f, %2.2f; a:%2.2f, %2.2f, %2.2f\n", imu_unit.Gyr.X, imu_unit.Gyr.Y, imu_unit.Gyr.Z
//                                                                  , imu_unit.Acc.X, imu_unit.Acc.Y, imu_unit.Acc.Z);
//        else
//          ky_info("pitch: %2.2f, roll: %2.2f, yaw: %2.2f  [%2d%%]\n", est_e.pitch, est_e.roll, est_e.yaw, osGetCPUUsage());
//        led_off(LED_BLUE);
//        ky_info("pitch: %2.2f, roll: %2.2f, yaw: %2.2f -%d-  [%2d%%]\n", est_e.pitch, est_e.roll, est_e.yaw, gyr_peace_flag, osGetCPUUsage());
      }
    }
  }
}

#define GYR_PEACE_THRESHOLD                      (0.2f) /* unit: dps */

static _3AxisUnit last_gyr = {0};
static void imu_peace_check(_3AxisUnit *pgyr)
{
  static uint32_t peace_ts = 0;
  static uint32_t peace_tn = 0;

  peace_tn = xTaskGetTickCount();
  if((fabs(pgyr->X - last_gyr.X) < GYR_PEACE_THRESHOLD) &&
     (fabs(pgyr->Y - last_gyr.Y) < GYR_PEACE_THRESHOLD) &&
     (fabs(pgyr->Z - last_gyr.Z) < GYR_PEACE_THRESHOLD)) {
    if(peace_tn - peace_ts > configTICK_RATE_HZ) { // keep 1s.
      if(gyr_peace_flag == 0) {
        gyr_peace_flag = 1;
//        icm42605_gyr_offset(pgyr);
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
