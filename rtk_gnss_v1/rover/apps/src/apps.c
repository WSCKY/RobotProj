#include "apps.h"
#include "test_case.h"

osThreadId LEDThread1Handle, LEDThread2Handle;

static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);

#define STR1(R) #R
#define STR2(R) STR1(R)

static const char SystemInfo[] =
"\n  .--,       .--,"
"\n ( (  \\.---./  ) )"
"\n  '.__/o   o\\__.'"
"\n     {=  ^  =}"               "\t\t\t"   "Integrated Navigation System based on SINS/GPS"
"\n      >  -  <"                "\t\t\t"   "Author:  kyChu<kychu@qq.com>"
"\n     /       \\"              "\t\t\t"   "Version: " STR2(__VERSION_STR__)
"\n    //       \\\\"            "\t\t\t"   "Date:    " __DATE__
"\n   //|   .   |\\\\"             "\t\t"   "Time:    " __TIME__
"\n   \"'\\       /'\"_.-~^`'-."     "\t"   "Board:   Deepblue RTK NAV Board(2019 V1)"
"\n      \\  _  /--'         `"      "\t"   "ALL RIGHTS RESERVED BY kyChu<kychu@qq.com>"
"\n    ___)( )(___"
"\n   (((__) (__)))"
"\n"
;

#define TEST_CASE_TASK_ENABLE                    (0)

void APP_StartThread(void const *argument)
{
  /* Initialize LEDs */
  leds_init();

  comif_init();
  log_init(comif_tx_string_util);
  osDelay(500);
  ky_alert("!!!KERNEL START!!!\n");
  comif_tx_string_util(SystemInfo);

  osThreadDef(SINS, att_est_q_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8); // stack size = 1KB
  osThreadDef(GNSS, gnss_navg_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
  osThreadDef(RTCM, rtcm_transfer_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
#if (TEST_CASE_TASK_ENABLE)
  osThreadDef(TEST, test_case_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8); // stack size = 1KB
#endif /* (TEST_CASE_TASK_ENABLE) */
  osThreadDef(MESG, mesg_send_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B

  if(osThreadCreate(osThread(SINS), NULL) == NULL) ky_err("sins task create failed.\n");
  if(osThreadCreate(osThread(GNSS), NULL) == NULL) ky_err("gnss task create failed.\n");
  if(osThreadCreate(osThread(RTCM), NULL) == NULL) ky_err("rtcm task create failed.\n");
#if (TEST_CASE_TASK_ENABLE)
  if(osThreadCreate(osThread(TEST), NULL) == NULL) ky_err("test task create failed.\n");
#endif /* (TEST_CASE_TASK_ENABLE) */
  if(osThreadCreate(osThread(MESG), NULL) == NULL) ky_err("mesg task create failed.\n");

  /* LED INDICATOR TASK */
  osThreadDef(LED_BLUE, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // stack size = 128B
  osThreadDef(LED_GREEN, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // stack size = 128B

  LEDThread1Handle = osThreadCreate(osThread(LED_BLUE), NULL); if(LEDThread1Handle == NULL) ky_err("led1 task create failed.\n");
  LEDThread2Handle = osThreadCreate(osThread(LED_GREEN), NULL); if(LEDThread2Handle == NULL) ky_err("led2 task create failed.\n");

  vTaskDelete(NULL);
}

/**
  * @brief  Toggle LED_RED thread
  * @param  Thread not used
  * @retval None
  */
static void LED_Thread1(void const *argument)
{
  uint32_t count = 0;
  (void) argument;
  
  for(;;)
  {
    count = osKernelSysTick() + 6000;
    
    /* Toggle LED_BLUE every 200 ms for 6 s */
    while (count > osKernelSysTick())
    {
      led_toggle(LED_BLUE);
      
      osDelay(200);
    }
    
    /* Turn off LED_BLUE */
    led_off(LED_BLUE);
    
    /* Suspend Thread 1 */
    osThreadSuspend(NULL);
    
    count = osKernelSysTick() + 6000;
    
    /* Toggle LED_RED every 400 ms for 6 s */
    while (count > osKernelSysTick())
    {
      led_toggle(LED_BLUE);

      osDelay(400);
    }
    
    /* Resume Thread 2 */
    osThreadResume(LEDThread2Handle); 
  }
}

/**
  * @brief  Toggle LED_GREEN thread
  * @param  argument not used
  * @retval None
  */
static void LED_Thread2(void const *argument)
{
  uint32_t count;
  (void) argument;
  
  for(;;)
  {
    count = osKernelSysTick() + 12000;
    
    /* Toggle LED_GREEN every 500 ms for 12 s */
    while (count > osKernelSysTick())
    {
      led_toggle(LED_GREEN);
      
      osDelay(500);
    }
    
    /* Turn off LED_GREEN */
    led_off(LED_GREEN);
    
    /* Resume Thread 1 */
    osThreadResume(LEDThread1Handle);

    /* Suspend Thread 2 */
    osThreadSuspend(NULL);  
  }
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
