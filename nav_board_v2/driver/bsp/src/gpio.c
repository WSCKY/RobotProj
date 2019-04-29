/*
 * gpio.c
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#include "gpio.h"

void gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(IMU_DEN_GPIO_PORT_CLK, ENABLE);
  /* Configure GPIO_PIN in output push-pull mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = IMU_DEN_GPIO_PORT_PIN;
  GPIO_Init(IMU_DEN_GPIO_PORT, &GPIO_InitStructure);

  GPIO_SetBits(IMU_DEN_GPIO_PORT, IMU_DEN_GPIO_PORT_PIN);
}
