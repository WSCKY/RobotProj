#include "nav_task.h"

UBX_ERROR ubx_init_flag = UBX_OK;
uint32_t ubx_cnt = 0;

void NAV_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  ubx_init_flag = ublox_m8q_init();
  for(;;) {
    osDelay(5);
    ubx_cnt ++;
  }
}
