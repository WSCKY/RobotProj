/*
 * pwm.c
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#include "pwm.h"

/**
  * @brief  Configure the TIM2 PWM Pin.
  * @param  None
  * @retval None
  */
void pwm_init(uint8_t d)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Clocks enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* GPIOA Configuration: GPIO_PIN_1 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_2);

  /* TIM2 Configuration ---------------------------------------------------
   Generate 1 PWM signal with 0% duty cycles:
   TIM2 input clock (TIM2CLK) is set to APB1 clock (PCLK1)
    => TIM2CLK = PCLK1 = SystemCoreClock
   TIM2CLK = SystemCoreClock, Prescaler = 47, TIM2 counter clock = 1MHz
   SystemCoreClock is set to 48 MHz for STM32F0xx devices

   TIM2_Period = (1MHz / 10KHz) - 1 = 99
   The channel 2 duty cycle is set to x%
   The Timer pulse is calculated as follows:
     - Channel2Pulse = DutyCycle * (TIM2_Period - 1) / 100
  ----------------------------------------------------------------------- */

  /* TIM2 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);

  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 47;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 99;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* Channel 2 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_Pulse = d;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC2Init(TIM2, &TIM_OCInitStructure);

  /* TIM2 counter enable */
  TIM_Cmd(TIM2, ENABLE);
}

void pwm_set_dutycycle(uint8_t d)
{
	if(d > 99) d = 99;
	TIM2->CCR2 = d;
}
