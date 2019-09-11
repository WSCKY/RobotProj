/**
  ******************************************************************************
  * @file    ./src/TimerCounter.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   This module provides a timing clock for system.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "TimerCounter.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t OverFlowCnt = 0;
static uint32_t TimeMeasure = 0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Configure the clock timer.
	* @param  None
	* @retval None
	*/
void _TimeTicksInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock) / 1000000) - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* NVIC configuration *******************************************************/
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = SYSTEM_TIMER_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Clear TIM3 Counter */
	TIM_SetCounter(TIM3, 0);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	/* TIM3 counter enable */
	TIM_Cmd(TIM3, ENABLE);
}

/*
 * return timer counter value.
 */
uint16_t _Get_Ticks(void) { return TIM3->CNT; }

/*
 * time in microsecond.
 */
uint32_t _Get_Micros(void)
{
	uint32_t t1, t2;
	do{
		t1 = (OverFlowCnt << 16) + (TIM3->CNT & 0xFFFF);
		t2 = (OverFlowCnt << 16) + (TIM3->CNT & 0xFFFF);
	} while(t1 != t2);
	return t1;
}

/*
 * time in millisecond.
 */
uint32_t _Get_Millis(void)
{
	return ((_Get_Micros()) / 1000);
}

/*
 * return seconds.
 */
uint32_t _Get_Secnds(void)
{
	return ((_Get_Millis()) / 1000);
}

/*
 * delay a time in microsecond.
 */
void _delay_us(uint32_t us)
{
	uint32_t _start = _Get_Micros();
	uint32_t _us = us;
	uint32_t _cur = _start;
	do {
		_cur = _Get_Micros();
	} while((_cur - _start) < _us);
}

/*
 * delay a time in millisecond.
 */
void _delay_ms(uint32_t ms)
{
	uint32_t _start = _Get_Millis();
	uint32_t _ms = ms;
	uint32_t _cur = _start;
	do {
		_cur = _Get_Millis();
	} while((_cur - _start) < _ms);
}

void _MeasureTimeStart(void)
{
  TimeMeasure = _Get_Micros();
}

uint32_t _GetTimeMeasured(void)
{
  return (_Get_Micros() - TimeMeasure);
}

/**
  * @brief  This function handles the TIM3 interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		OverFlowCnt ++;
	}
}

/******************************** END OF FILE *********************************/
