#include "imu_task.h"

void IMU_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);

  QueueHandle_t* q_mpu_raw = mpu_queue_get();
  osThreadDef(CAL, CalibTask, osPriorityHigh, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(CAL), q_mpu_raw);

  vTaskDelete(NULL);
}
