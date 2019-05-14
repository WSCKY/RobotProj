/**
  ******************************************************************************
  * @file    Board/main.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    20-June-2018
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
osThreadId START_ThreadId;
osThreadId APP_ThreadId;
osThreadId NAV_ThreadId;
osThreadId IMU_ThreadId;
osThreadId COM_ThreadId;
osThreadId TST_ThreadId;

int init_ret = 0;

/* Private function prototypes -----------------------------------------------*/
static void StartThread(void const *argument);
static void APP_Thread(void const *argument);
static void TST_Thread(void const *argument);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< System Main Entry Point. */
  NVIC_PriorityGroupConfig(SYSTEM_PRIORITY_GROUP_CONFIG);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* System timer clock */
  _TimeTicksInit();

  /* Configure Systick for RTOS Kernel */
  SysTick_Config(SystemCoreClock / 1000);
  NVIC_SetPriority(SysTick_IRQn, INT_PRIORITY_SYSTICK);

  /* Thread definition */
  osThreadDef(START, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  /* Start thread */
  START_ThreadId = osThreadCreate(osThread(START), NULL);

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  for(;;);
}

static void StartThread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  init_ret = bsp_init();

  /* Thread definition */
  osThreadDef(IMU, IMU_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(NAV, NAV_Thread, NAV_TASK_PRIORITY, 0, NAV_TASK_STACK_SIZE);
  osThreadDef(COM, COM_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(LED, APP_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(TST, TST_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  /* Start thread */
  if(init_ret == 0) {
    IMU_ThreadId = osThreadCreate(osThread(IMU), NULL);
    NAV_ThreadId = osThreadCreate(osThread(NAV), NULL);
  }
  COM_ThreadId = osThreadCreate(osThread(COM), NULL);
  APP_ThreadId = osThreadCreate(osThread(LED), NULL);
  TST_ThreadId = osThreadCreate(osThread(TST), NULL);

  vTaskDelete(NULL);
  for(;;);
}

/**
  * @brief  Toggle LED thread
  * @param  Thread not used
  * @retval None
  */
static void APP_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  uint32_t PreviousWakeTime = osKernelSysTick();

  for(;;)
  {
    LED_R_ON(); osDelayUntil (&PreviousWakeTime, 100);
    LED_R_OFF(); osDelayUntil (&PreviousWakeTime, 400);
if(init_ret == 0) {
    LED_G_ON(); osDelayUntil (&PreviousWakeTime, 100);
    LED_G_OFF(); osDelayUntil (&PreviousWakeTime, 400);

    LED_B_ON(); osDelayUntil (&PreviousWakeTime, 100);
    LED_B_OFF(); osDelayUntil (&PreviousWakeTime, 400);
}
  }
}

static void TST_Thread(void const *argument)
{
	for(;;) {
		osDelay(50);
	}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
