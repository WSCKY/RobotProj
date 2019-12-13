#include "test_case.h"
#include <stdio.h>
#include "ringbuffer.h"
#include <string.h>
#include "cpu_utils.h"

//static ringbuffer_handle test_rb;
//#define RB_TEST_CAPACITY       256
//static const uint8_t test_str[] = "abcdefghijklmnopqrstuvwxyz0123456789.\n"; // 38 bytes.
//static uint8_t rb_test_cache[RB_TEST_CAPACITY];

//static void rb_tx_task(void const *argument);
//static void rb_rx_task(void const *argument);
//uint16_t last_ts = 0;
//uint16_t delta_ts = 0;
IMU_RAW_6DOF Raw;
IMU_UNIT_6DOF Unit;

void test_case_task(void const *argument)
{
  (void) argument;
  ky_info("test case task start.\n");
  osDelay(500);
  if(icm42605_init() != status_ok) {
	  ky_err("icm42605 init failed.\n");
  }
//  ringbuffer_init(&test_rb, rb_test_cache, RB_TEST_CAPACITY);
//  
//  osThreadDef(RB_TX, rb_tx_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//  osThreadDef(RB_RX, rb_rx_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//  if(osThreadCreate(osThread(RB_TX), NULL) == NULL) dbg_str("rb tx test task failed.\n");
//  if(osThreadCreate(osThread(RB_RX), NULL) == NULL) dbg_str("rb rx test task failed.\n");
  uint32_t ts = 0, tn = 0, ts_cnt = 0;
  uint32_t last_t = 0, delta_t = 0;
  for(;;) {
//    osDelay(1000);
//    delta_ts = imu_ts - last_ts;
//    last_ts = imu_ts;
    if(icm42605_read(&Raw, &Unit, osWaitForever) ==status_ok) {
    if(Raw.TS > last_t)
      delta_t = Raw.TS - last_t;
    else
    	delta_t = 0x10000 - last_t + Raw.TS;
    last_t = Raw.TS;

    tn = xTaskGetTickCountFromISR();
      if((tn - ts) > 50) {
        ts = tn;
        if(delta_t != 1000) {
        	ky_info("t_err: ts:%d, delta:%d  [%2d%%]\n", Raw.TS, delta_t, osGetCPUUsage());
        } else {
          ts_cnt ++;
          if(ts_cnt >= 20) {
        	  ts_cnt = 0;
        	  ky_info("ax: %2.2f, ay: %2.2f, az: %2.2f, %2.2fC, ts:%d  [%2d%%]\n", \
        	          		Unit.Acc.X, Unit.Acc.Y, Unit.Acc.Z, Unit.Temp, delta_t, osGetCPUUsage());
          }
        }
      }
    }
  }
}

//uint8_t over_flow = 0;
//uint32_t percent_in_use = 0;

//bool_t rb_in_use = false;

//static void rb_tx_task(void const *argument)
//{
//  (void) argument;
//  uint32_t len = 0;
//  uint8_t rx_cache[32];
//  uint32_t test_cnt = 0;
//  for(;;) {
//    osDelay(200);
//    rb_in_use = true;
//    if(ringbuffer_push(&test_rb, (uint8_t *)test_str, 38) < 38)
//      over_flow ++;
//    rb_in_use = false;
//    test_cnt ++;
//    if(test_cnt >= 5) {
//      test_cnt = 0;
//      percent_in_use = ringbuffer_available(&test_rb) * 100 / RB_TEST_CAPACITY;
//      while((len = ringbuffer_poll(&test_rb, rx_cache, 32)) > 0) {
//        comif_tx_bytes_util(rx_cache, len);
//      }
//    }
//  }
//}

//static void rb_rx_task(void const *argument)
//{
//  (void) argument;
////  uint32_t len = 0;
////  uint8_t rx_cache[32];
//  for(;;) {
//    osDelay(1000);
////    while(rb_in_use == true) {osDelay(1);}
////    percent_in_use = ringbuffer_available(&test_rb) * 100 / RB_TEST_CAPACITY;
////    while((len = ringbuffer_poll(&test_rb, rx_cache, 32)) > 0) {
////      comif_tx_bytes_util(rx_cache, len);
////    }
//  }
//}

//static void GSM_Thread(void const *argument);

//uint8_t buffer_comp(uint8_t *src, uint8_t *dst, uint32_t l)
//{
//  while(l --) {
//    if(*src != *dst)
//      return 0;
//    else {
//      src ++;
//      dst ++;
//    }
//  }
//  return 1;
//}

//uint8_t comrxbf[32];
//static void GSM_Thread(void const *argument)
//{
//  (void) argument;

//  ec20_power_on();
//  osDelay(3000);
//  ec20if_init();
//  dbg_str("ec20 uart init.\n");

//  dbg_str("ec20 wait poweron.\n");
//  if(ec20_check_ack((uint8_t *)"RDY\r\n", 8000) == status_ok) // wait 8s for ec20 power on.
//    dbg_str("ec20 powered.\n");
//  else {
//    for(;;) {
//      dbg_str("EC20 Power on FAIL!\n");
//      osDelay(1000);
//    }
//  }

//  osDelay(1000);
//  if(ec20_check_cmd_ack((uint8_t *)"AT\r\n", (uint8_t *)"AT\r\r\nOK\r\n", 5, 1000) == status_ok)
//    dbg_str("ec20 test done.\n");
//  else
//    dbg_str("ec20 test fail.\n");
//  osDelay(1000);
//  if(ec20_check_cmd_ack((uint8_t *)"AT+QIOPEN=1,0,\"TCP\",\"36.153.88.121\",8086,0,2\r\n", (uint8_t *)"\r\r\nCONNECT\r\n", 5, 1000) == status_ok)
//    dbg_str("ec20 start done.\n");
//  else
//    dbg_str("ec20 start fail.\n");

//  for(;;) {
//    osDelay(1);
//    if(comif_rx_bytes(comrxbf, 32) > 0) {
//      if(buffer_comp(comrxbf, (uint8_t *)"TEST", 4)) {
//        if(ec20_check_cmd_ack((uint8_t *)"AT\r\n", (uint8_t *)"AT\r\r\nOK\r\n", 5, 1000) == status_ok)
//          dbg_str("ec20 test done.\n");
//        else
//          dbg_str("ec20 test fail.\n");
//      }
//      if(buffer_comp(comrxbf, (uint8_t *)"START", 5)) {
//        if(ec20_check_cmd_ack((uint8_t *)"AT+QIOPEN=1,0,\"TCP\",\"36.153.88.121\",8086,0,2\r\n", (uint8_t *)"\r\r\nCONNECT\r\n", 5, 1000) == status_ok)
//          dbg_str("ec20 start done.\n");
//        else
//          dbg_str("ec20 start fail.\n");
//      }
//      if(buffer_comp(comrxbf, (uint8_t *)"EXIT", 4)) {
//        if(ec20_check_cmd_ack((uint8_t *)"+++", (uint8_t *)"\r\nOK\r\n", 5, 1200) == status_ok) // should be > 1s
//          dbg_str("ec20 exit done.\n");
//        else
//          dbg_str("ec20 exit fail.\n");
//      }
//      if(buffer_comp(comrxbf, (uint8_t *)"CLOSE", 5)) {
//        if(ec20_check_cmd_ack((uint8_t *)"AT+QICLOSE=0\r\n", (uint8_t *)"\r\r\nOK\r\n", 5, 1000) == status_ok)
//          dbg_str("ec20 close done.\n");
//        else
//          dbg_str("ec20 close fail.\n");
//      }
//    }
//  }
//}
