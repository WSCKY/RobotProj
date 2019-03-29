/**
  ******************************************************************************
  * @file    ./startup/start.c
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "SysConfig.h"
#include "boardconfig.h"
#if FREERTOS_ENABLED
#include "cmsis_os.h"
#endif /* FREERTOS_ENABLED */
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main_app(void)
{
#if BOOT_ENABLED
	memcpy((void *)SYS_SRAM_ORIGIN, (void*)SYS_TEXT_ORIGIN, SYS_VECTOR_SIZE);
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
#endif /* BOOT_ENABLED */
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f0xx.c file
     */
	SystemCoreClockUpdate();
#if SYSTICK_ENABLE
	SysTick_Config(SystemCoreClock / 1000);
#endif /* SYSTICK_ENABLE */
	/* Enable Syscfg */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	_TimeTicksInit();
#if FREERTOS_ENABLED
	osThreadDef(Start, StartThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	osThreadCreate (osThread(Start), NULL);
	/* Start scheduler */
	osKernelStart();
#else
	StartThread(0);
#endif /* FREERTOS_ENABLED */
	/* We should never get here as control is now taken by the scheduler */
	for( ;; );
	return 0;
}

#ifdef  USE_FULL_ASSERT

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

/******************************** END OF FILE *********************************/
