/**
  ******************************************************************************
  * @file    bsp/leds.c
  * @author  kyChu<kychu@qq.com>
  * @brief   LED driver.
  ******************************************************************************
  */

#include "leds.h"

void leds_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_RESET);
}

void led_on(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);
  } else {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET); 
  }
}

void led_off(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
  } else {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);            
  }
}

void led_toggle(Led_TypeDef led)
{
  if(led == LED1) {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
  } else {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
  }
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
