#include "AttitudeEst.h"
#include "MathFunction.h"

static Quat_T AttQ = {1, 0, 0, 0};
static Euler_T AttE = {0, 0, 0};
static float fusionDt = 0.001f;
static float prop_gain_kp = 3.0f, intg_gain_ki = 0.0f;

/* debug data. */
static COM_MSG_DEF dbg_msg;
static void debug_msg_init(void);
static void com_msg_update(void);

void AttitudeEstTask(void const *argument)
{
  IMU_UNIT imu_unit;
  uint32_t lastTimeStamp = 0;
  QueueHandle_t q_mpu = *((QueueHandle_t *)argument);

  debug_msg_init();
  for(;;) {
    if(xQueueReceive(q_mpu, (void *)&imu_unit, pdMS_TO_TICKS(2)) == pdPASS) {
      if(lastTimeStamp == 0) {
        lastTimeStamp = imu_unit.TimeStamp;
        fusionDt = 0.001f;
      } else {
        fusionDt = (float)(imu_unit.TimeStamp - lastTimeStamp) / 1000000.0f;
        lastTimeStamp = imu_unit.TimeStamp;
      }
      fusionQ_6dot(&imu_unit, &AttQ, prop_gain_kp, intg_gain_ki, fusionDt);
      com_msg_update();
      Quat2Euler(&AttQ, &AttE);
    }
  }
}

static void debug_msg_init(void)
{
  dbg_msg.len = sizeof(Quat_T);
  dbg_msg.type = TYPE_ATT_QUAT_Resp;
  dbg_msg.pointer = (void *)&AttQ;
}

static void com_msg_update(void)
{
  static uint32_t time_stamp = 0;
  static QueueHandle_t q_com = NULL;
  if(q_com == NULL) {
    q_com = *get_com_msg_send_queue();
    return;
  }
  if(_Get_Millis() - time_stamp > 20) {
    xQueueSend(q_com, (void *)&dbg_msg, 1);
    time_stamp = _Get_Millis();
  }
}
