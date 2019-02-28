#include "imu_calib.h"
#include "maths.h"

static IMU_RAW raw;
static IMU_RAW calib_raw;
static IMU_UNIT unit;
static GyrRawDef GyrOffset;
static uint8_t gyr_calib_flag = 0; /* imu calibrated flag */

static QueueHandle_t q_mpu_unit = NULL; /* queue to send message to estimator */

/* debug data. */
//static COM_MSG_DEF dbg_msg;
//static IMU_INFO_DEF imu_info;
//static void debug_msg_init(void);
//static void com_msg_update(void);

static void calib_loop(void);

void CalibTask(void const *argument)
{
  QueueHandle_t q_mpu = *((QueueHandle_t *)argument);

//  debug_msg_init();

  q_mpu_unit = xQueueCreate(2, sizeof(IMU_UNIT));
  osThreadDef(EST, AttitudeEstTask, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 4);
  osThreadCreate(osThread(EST), &q_mpu_unit);

  for(;;) {
    if(xQueueReceive(q_mpu, (void *)&raw, pdMS_TO_TICKS(2)) == pdPASS) {
      if(gyr_calib_flag != 0) {
        calib_raw.gyrX = raw.gyrX - GyrOffset.gyrX;
        calib_raw.gyrY = raw.gyrY - GyrOffset.gyrY;
        calib_raw.gyrZ = raw.gyrZ - GyrOffset.gyrZ;
        calib_raw.accX = raw.accX;
        calib_raw.accY = raw.accY;
        calib_raw.accZ = raw.accZ;
        mpu_raw2unit(&calib_raw, &unit);
        xQueueSend(q_mpu_unit, (void *)&unit, 5);
      }

      calib_loop();

//      com_msg_update();
    }
  }
}

#define CALIB_BUFF_SIZE                          (100)
static GyrRawDef GyrPeaceBuf[CALIB_BUFF_SIZE] = {0};
static uint8_t PeaceDataCnt = 0, PeaceDataIndex = 0;

static void calib_loop(void)
{
  static uint32_t TimeStart = 0;
  static uint32_t TimeCurrent = 0, TimeStampDiv = 0;
  static int16_t gyrX = 0, gyrY = 0, gyrZ = 0;

  TimeCurrent = _Get_Millis();
  if(TimeCurrent - TimeStampDiv < 10) return; /* calib task divider. */
  TimeStampDiv = TimeCurrent;

  uint16_t turbulence = ABS(raw.gyrX - gyrX) + ABS(raw.gyrY - gyrY) + ABS(raw.gyrZ - gyrZ);
  gyrX = raw.gyrX; gyrY = raw.gyrY; gyrZ = raw.gyrZ;
  if(TimeStart == 0) {
    TimeStart = TimeCurrent; // init time stamp.
  }
  if(turbulence < 15) {
    if((TimeCurrent - TimeStart) > 2000) { // keep 1 second.
      if(gyr_calib_flag == 0) {
        gyr_calib_flag = 1;
        GyrOffset.gyrX = raw.gyrX;
        GyrOffset.gyrY = raw.gyrY;
        GyrOffset.gyrZ = raw.gyrZ;
      } else {
        GyrPeaceBuf[PeaceDataIndex].gyrX = raw.gyrX;
        GyrPeaceBuf[PeaceDataIndex].gyrY = raw.gyrY;
        GyrPeaceBuf[PeaceDataIndex].gyrZ = raw.gyrZ;
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
//
//static void debug_msg_init(void)
//{
//  dbg_msg.len = sizeof(IMU_INFO_DEF);
//  dbg_msg.type = TYPE_IMU_INFO_Resp;
//  dbg_msg.pointer = (void *)&imu_info;
//}
//
//static void com_msg_update(void)
//{
//  static uint32_t time_stamp = 0;
//  static QueueHandle_t q_com = NULL;
//  if(q_com == NULL) {
//    q_com = *get_com_msg_send_queue();
//    return;
//  }
//  if(_Get_Millis() - time_stamp > 20) {
//    imu_info.accX = calib_raw.accX;
//    imu_info.accY = calib_raw.accY;
//    imu_info.accZ = calib_raw.accZ;
//    imu_info.gyrX = calib_raw.gyrX;
//    imu_info.gyrY = calib_raw.gyrY;
//    imu_info.gyrZ = calib_raw.gyrZ;
//    xQueueSend(q_com, (void *)&dbg_msg, 1);
//    time_stamp = _Get_Millis();
//  }
//}
