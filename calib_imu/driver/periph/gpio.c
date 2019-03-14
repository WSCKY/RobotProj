/*
 * gpio.c
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#include "gpio.h"

void user_io_init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	/* Enable GPIOB clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
