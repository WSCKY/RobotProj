/**
  ******************************************************************************
  * @file    ./src/main.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   LED Driver Module.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "LED.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  set LED Driver Pin Initialization.
  * @param  None
  * @retval None
  */
void LED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* LED_GPIO Periph clock enable */
  RCC_AHBPeriphClockCmd(LED_B_GPIO_PORT_CLK | LED_R_GPIO_PORT_CLK, ENABLE);

  /* Configure LED_GPIO_Pin in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = LED_B_GPIO_PORT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(LED_B_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LED_R_GPIO_PORT_PIN;
  GPIO_Init(LED_R_GPIO_PORT, &GPIO_InitStructure);
}

/******************************** END OF FILE *********************************/
