/** 
  ******************************************************************************
  * @file    stm32f7xx_rtk_gps_nav.h
  * @author  kyChu
  * @version V1.0.0
  * @date    20-August-2019
  * @brief   This file contains definitions for:
  *          - LEDs available on STM32F7XX-RTK-GPS-NAV Board 
  *            from Deepblue
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F7XX_RTK_GPS_NAV_H
#define __STM32F7XX_RTK_GPS_NAV_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/** STM32F7XX_RTK_GPS_NAV_LOW_LEVEL_Exported_Types
  */
typedef enum 
{
  LED1 = 0,
  LED_BLUE = LED1,
  LED2 = 1,
  LED_GREEN = LED2,
}Led_TypeDef;

/** STM32F7XX_RTK_GPS_NAV_LOW_LEVEL_Exported_Constants
  */ 

/** 
  * Define for STM32F7XX_RTK_GPS_NAV board  
  */ 
#if !defined (USE_STM32F7XX_RTK_GPS_NAV)
 #define USE_STM32F7XX_RTK_GPS_NAV
#endif

/** STM32F7XX_RTK_GPS_NAV_LOW_LEVEL_LED
  */
#define LEDn                                    2

#define LED1_PIN                                GPIO_PIN_1
#define LED1_GPIO_PORT                          GPIOB
#define LED1_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define LED2_PIN                                GPIO_PIN_2
#define LED2_GPIO_PORT                          GPIOB
#define LED2_GPIO_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {__HAL_RCC_GPIOB_CLK_ENABLE();} else\
                                                                    {__HAL_RCC_GPIOB_CLK_ENABLE();   }} while(0)	
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do { if((__INDEX__) == 0) {__HAL_RCC_GPIOB_CLK_DISABLE();} else\
                                                                    {__HAL_RCC_GPIOB_CLK_DISABLE();   }} while(0)	

/** STM32F7XX_RTK_GPS_NAV_LOW_LEVEL_Exported_Macros
  */

/** STM32F7XX_RTK_GPS_NAV_LOW_LEVEL_Exported_Functions
  */
uint32_t         BSP_GetVersion(void);
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7XX_RTK_GPS_NAV_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
