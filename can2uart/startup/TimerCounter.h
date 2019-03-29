/**
  ******************************************************************************
  * @file    ./inc/TimerCounter.h 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Header for TimerCounter.c module.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIMERCOUNTER_H
#define __TIMERCOUNTER_H

/* Includes ------------------------------------------------------------------*/
#include "SysConfig.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void _TimeTicksInit(void);

uint16_t _Get_Ticks(void);
uint32_t _Get_Micros(void);
uint32_t _Get_Millis(void);
uint32_t _Get_Secnds(void);

void _delay_us(uint32_t us);
void _delay_ms(uint32_t ms);

void _MeasureTimeStart(void);
uint32_t _GetTimeMeasured(void);

#endif /* __TIMERCOUNTER_H */

/******************************** END OF FILE *********************************/
