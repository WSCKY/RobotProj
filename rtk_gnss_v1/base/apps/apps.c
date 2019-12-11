#include "apps.h"
#include "test_case.h"

osThreadId LEDThread1Handle, LEDThread2Handle;

static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);

void APP_StartThread(void const *argument)
{
  /* Initialize LEDs */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);

  comif_init();
  osDelay(500);
  dbg_str("system start.\n");
  osThreadDef(LED_BLUE, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(LED_GREEN, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(GNSS, gnss_navg_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//  osThreadDef(TEST, test_case_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  if(osThreadCreate(osThread(GNSS), NULL) == NULL) dbg_str("gnss task create failed.\n");
//  if(osThreadCreate(osThread(TEST), NULL) == NULL) dbg_str("test task create failed.\n");
  LEDThread1Handle = osThreadCreate(osThread(LED_BLUE), NULL); if(LEDThread1Handle == NULL) dbg_str("led1 task create failed.\n");
  LEDThread2Handle = osThreadCreate(osThread(LED_GREEN), NULL); if(LEDThread2Handle == NULL) dbg_str("led2 task create failed.\n");

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
      BSP_LED_Toggle(LED_BLUE);
      
      osDelay(200);
    }
    
    /* Turn off LED_BLUE */
    BSP_LED_Off(LED_BLUE);
    
    /* Suspend Thread 1 */
    osThreadSuspend(NULL);
    
    count = osKernelSysTick() + 6000;
    
    /* Toggle LED_RED every 400 ms for 6 s */
    while (count > osKernelSysTick())
    {
      BSP_LED_Toggle(LED_BLUE);

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
      BSP_LED_Toggle(LED_GREEN);
      
      osDelay(500);
    }
    
    /* Turn off LED_GREEN */
    BSP_LED_Off(LED_GREEN);
    
    /* Resume Thread 1 */
    osThreadResume(LEDThread1Handle);

    /* Suspend Thread 2 */
    osThreadSuspend(NULL);  
  }
}
