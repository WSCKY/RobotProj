#include "apps.h"
#include "test_case.h"

osThreadId LEDThread1Handle, LEDThread2Handle;

static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);

void APP_StartThread(void const *argument)
{
  /* Initialize LEDs */
  leds_init();

  comif_init();
  log_init(comif_tx_string_util);
  osDelay(500);
  ky_alert("system start.\n");

  osThreadDef(GNSS, gnss_navg_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadDef(RTCM, rtcm_transfer_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadDef(TEST, test_case_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 8);
#if !DEBUG_ENABLE
  osThreadDef(MESG, mesg_send_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
#endif
  osThreadDef(LED_BLUE, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(LED_GREEN, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  if(osThreadCreate(osThread(GNSS), NULL) == NULL) ky_err("gnss task create failed.\n");
  if(osThreadCreate(osThread(RTCM), NULL) == NULL) ky_err("rtcm transfer task failed.\n");
  if(osThreadCreate(osThread(TEST), NULL) == NULL) ky_err("test task create failed.\n");
#if !DEBUG_ENABLE
  if(osThreadCreate(osThread(MESG), NULL) == NULL) ky_err("mesg transfer task failed.\n");
#endif
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
