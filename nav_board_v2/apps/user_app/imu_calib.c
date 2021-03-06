#include "imu_calib.h"
#include "MathFunction.h"

static IMU_RAW raw;
static IMU_RAW calib_raw;
static IMU_UNIT unit;
static _3AxisRaw GyrOffset;
static uint8_t gyr_calib_flag = 0; /* imu calibrated flag */

static QueueHandle_t q_mpu_unit = NULL; /* queue to send message to estimator */

/* debug data. */
static COM_MSG_DEF dbg_msg;
static IMU_9DOF_DEF imu_info;
static void debug_msg_init(void);
static void com_msg_update(void);

static void calib_loop(void);

void CalibTask(void const *argument)
{
  QueueHandle_t q_mpu = *((QueueHandle_t *)argument);

  debug_msg_init();

  q_mpu_unit = xQueueCreate(5, sizeof(IMU_UNIT));
  osThreadDef(EST, AttitudeEstTask, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 4);
  osThreadCreate(osThread(EST), &q_mpu_unit);

  for(;;) {
    if(xQueueReceive(q_mpu, (void *)&raw, pdMS_TO_TICKS(2)) == pdPASS) {
      if(gyr_calib_flag != 0) {
        calib_raw.Gyr.X = raw.Gyr.X - GyrOffset.X;
        calib_raw.Gyr.Y = raw.Gyr.Y - GyrOffset.Y;
        calib_raw.Gyr.Z = raw.Gyr.Z - GyrOffset.Z;
        calib_raw.Acc.X = raw.Acc.X;
        calib_raw.Acc.Y = raw.Acc.Y;
        calib_raw.Acc.Z = raw.Acc.Z;
        imu_raw2unit(&calib_raw, &unit);
        xQueueSend(q_mpu_unit, (void *)&unit, 5);
      }

      calib_loop();

      com_msg_update();
    }
  }
}

#define CALIB_BUFF_SIZE                          (100)
static _3AxisRaw GyrPeaceBuf[CALIB_BUFF_SIZE] = {0};
static uint8_t PeaceDataCnt = 0, PeaceDataIndex = 0;

static void calib_loop(void)
{
  static uint32_t TimeStart = 0;
  static uint32_t TimeCurrent = 0, TimeStampDiv = 0;
  static int16_t gyrX = 0, gyrY = 0, gyrZ = 0;

  TimeCurrent = _Get_Millis();
  if(TimeCurrent - TimeStampDiv < 10) return; /* calib task divider. */
  TimeStampDiv = TimeCurrent;

  uint16_t turbulence = ABS(raw.Gyr.X - gyrX) + ABS(raw.Gyr.Y - gyrY) + ABS(raw.Gyr.Z - gyrZ);
  gyrX = raw.Gyr.X; gyrY = raw.Gyr.Y; gyrZ = raw.Gyr.Z;
  if(TimeStart == 0) {
    TimeStart = TimeCurrent; // init time stamp.
  }
  if(turbulence < 25) {
    if((TimeCurrent - TimeStart) > 2000) { // keep 1 second.
      if(gyr_calib_flag == 0) {
        gyr_calib_flag = 1;
        GyrOffset.X = raw.Gyr.X;
        GyrOffset.Y = raw.Gyr.Y;
        GyrOffset.Z = raw.Gyr.Z;
      } else {
        GyrPeaceBuf[PeaceDataIndex].X = raw.Gyr.X;
        GyrPeaceBuf[PeaceDataIndex].Y = raw.Gyr.Y;
        GyrPeaceBuf[PeaceDataIndex].Z = raw.Gyr.Z;
        PeaceDataIndex ++;
        if(PeaceDataIndex >= CALIB_BUFF_SIZE)
          PeaceDataIndex = 0;
        if(PeaceDataCnt < CALIB_BUFF_SIZE) // fill the buffer first.
          PeaceDataCnt ++;
        else {
          GyrOffset.X = GyrPeaceBuf[PeaceDataIndex].X;
          GyrOffset.Y = GyrPeaceBuf[PeaceDataIndex].Y;
          GyrOffset.Z = GyrPeaceBuf[PeaceDataIndex].Z;
        }
      }
    }
  } else {
    TimeStart = TimeCurrent;
  }
}

static void debug_msg_init(void)
{
  dbg_msg.len = sizeof(IMU_9DOF_DEF);
  dbg_msg.type = TYPE_IMU_9DOF_Resp;
  dbg_msg.pointer = (void *)&imu_info;
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
    imu_info.accX = raw.Acc.X;
    imu_info.accY = raw.Acc.Y;
    imu_info.accZ = raw.Acc.Z;
    imu_info.gyrX = raw.Gyr.X;
    imu_info.gyrY = raw.Gyr.Y;
    imu_info.gyrZ = raw.Gyr.Z;
    imu_info.magX = raw.Mag.X;
    imu_info.magY = raw.Mag.Y;
    imu_info.magZ = raw.Mag.Z;
    xQueueSend(q_com, (void *)&dbg_msg, 1);
    time_stamp = _Get_Millis();
  }
}
