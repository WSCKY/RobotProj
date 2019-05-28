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

osSemaphoreId btnSemaphore;
//static uint8_t TestWrite[12] = "hskcheudktls";
static uint8_t TestRead[44] = {0};
/* Private function prototypes -----------------------------------------------*/
static void bsp_init(void);
static void StartThread(void const *argument);
static void APP_Thread(void const *argument);
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
  bsp_init();

  /* Thread definition */
  osThreadDef(IMU, IMU_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	osThreadDef(GUI, NAV_Thread, NAV_TASK_PRIORITY, 0, NAV_TASK_STACK_SIZE);
  osThreadDef(COM, COM_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(LED, APP_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  /* Start thread */
  IMU_ThreadId = osThreadCreate(osThread(IMU), NULL);
	NAV_ThreadId = osThreadCreate(osThread(GUI), NULL);
  COM_ThreadId = osThreadCreate(osThread(COM), NULL);
  APP_ThreadId = osThreadCreate(osThread(LED), NULL);

  vTaskDelete(NULL);
  for(;;);
}

static void bsp_init(void)
{
  uint16_t num_r = 44;
  PWR_CTRL_Init(); /* Configure power control pin */
  PWR_IMU_ON(); /* Power on IMU Sensor */
  PWR_3V3_ON(); /* Power on GPS module and EEPROM */
  osDelay(5); /* Wait for power stabled */

  LED_Init(); /* initialize led drive pin */
	LED_RED_OFF(); LED_BLUE_OFF(); LED_GREEN_OFF();

  btnSemaphore = ButtonInit();

  sEE_Init();
//  sEE_WriteBuffer(TestWrite, 2, 12);
  osDelay(5);
  sEE_ReadBuffer(TestRead, 0, &num_r);
  HeaterDrvInit();
  mpu9250_init();
}
extern uint8_t mag_id;
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
/* osDelayUntil function differs from osDelay() in one important aspect:  osDelay () will
 * cause a thread to block for the specified time in ms from the time osDelay () is
 * called.  It is therefore difficult to use osDelay () by itself to generate a fixed
 * execution frequency as the time between a thread starting to execute and that thread
 * calling osDelay () may not be fixed [the thread may take a different path though the
 * code between calls, or may get interrupted or preempted a different number of times
 * each time it executes].
 *
 * Whereas osDelay () specifies a wake time relative to the time at which the function
 * is called, osDelayUntil () specifies the absolute (exact) time at which it wishes to
 * unblock.
 * PreviousWakeTime must be initialised with the current time prior to its first use 
 * (PreviousWakeTime = osKernelSysTick() )   
 */
    LED_RED_ON();
    osDelayUntil (&PreviousWakeTime, 100);
    LED_RED_OFF();
    osDelayUntil (&PreviousWakeTime, 400);
    if(mag_id == 0x48) {
    LED_BLUE_ON();
		osDelayUntil (&PreviousWakeTime, 100);
    LED_BLUE_OFF();
		osDelayUntil (&PreviousWakeTime, 400);
    LED_GREEN_ON();
    osDelayUntil (&PreviousWakeTime, 100);
    LED_GREEN_OFF();
    osDelayUntil (&PreviousWakeTime, 400);
    }
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
