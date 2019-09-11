/*
 * intio.c
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#include "intio.h"

static INTIO_IRQCallback INTxIRQCallback = 0;

void intio_init(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Enable GPIOB clock */
	RCC_AHBPeriphClockCmd(INTx_GPIO_CLK, ENABLE);

	/* Configure INT pin as output pull up. */
	GPIO_InitStructure.GPIO_Pin = INTx_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(INTx_GPIO_PORT, &GPIO_InitStructure);

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTIx Line to IRQ pin */
	SYSCFG_EXTILineConfig(INTx_GPIO_PortSource, INTx_GPIO_PinSource);

	/* Configure EXTIx line */
	EXTI_InitStructure.EXTI_Line = INTx_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTIx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = INTx_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = IMU_UPDATE_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void intio_set_irq_handler(INTIO_IRQCallback p)
{
	if(p != 0) {
		INTxIRQCallback = p;
	}
}

void INTx_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(INTx_EXTI_LINE) != RESET) {
		if(INTxIRQCallback != 0) {
			INTxIRQCallback();
		}
		EXTI_ClearITPendingBit(INTx_EXTI_LINE);
	}
}
