/**
  ******************************************************************************
  * @file    ./inc/LED.h 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Header for LED.c module.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LED_H
#define __LED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define LED_B_GPIO_PORT                GPIOB
#define LED_B_GPIO_PORT_PIN            GPIO_Pin_0
#define LED_B_GPIO_PORT_CLK            RCC_AHBPeriph_GPIOB

#define LED_R_GPIO_PORT                GPIOB
#define LED_R_GPIO_PORT_PIN            GPIO_Pin_1
#define LED_R_GPIO_PORT_CLK            RCC_AHBPeriph_GPIOB

/* Exported macro ------------------------------------------------------------*/
#define LED_B_ON()                     do { LED_B_GPIO_PORT->BSRR = LED_B_GPIO_PORT_PIN; } while(0);
#define LED_B_OFF()                    do { LED_B_GPIO_PORT->BRR = LED_B_GPIO_PORT_PIN; } while(0);
#define LED_B_TOG()                    do { LED_B_GPIO_PORT->ODR ^= LED_B_GPIO_PORT_PIN; } while(0);

#define LED_R_ON()                     do { LED_R_GPIO_PORT->BSRR = LED_R_GPIO_PORT_PIN; } while(0);
#define LED_R_OFF()                    do { LED_R_GPIO_PORT->BRR = LED_R_GPIO_PORT_PIN; } while(0);
#define LED_R_TOG()                    do { LED_R_GPIO_PORT->ODR ^= LED_R_GPIO_PORT_PIN; } while(0);

/* Exported functions ------------------------------------------------------- */
void LED_Init(void);

#endif /* __LED_H */

/******************************** END OF FILE *********************************/
