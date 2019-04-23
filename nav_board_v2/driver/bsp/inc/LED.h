#ifndef __LED_H
#define __LED_H

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define LED_RED_PORT                   GPIOB
#define LED_RED_PORT_PIN               GPIO_Pin_14
#define LED_BLUE_PORT                  GPIOB
#define LED_BLUE_PORT_PIN              GPIO_Pin_12
#define LED_GREEN_PORT                 GPIOB
#define LED_GREEN_PORT_PIN             GPIO_Pin_13

#define LED_RED_ON()                   LED_RED_PORT->BSRRH = LED_RED_PORT_PIN
#define LED_RED_OFF()                  LED_RED_PORT->BSRRL = LED_RED_PORT_PIN
#define LED_RED_TOG()                  LED_RED_PORT->ODR ^= LED_RED_PORT_PIN

#define LED_BLUE_ON()                  LED_BLUE_PORT->BSRRH = LED_BLUE_PORT_PIN
#define LED_BLUE_OFF()                 LED_BLUE_PORT->BSRRL = LED_BLUE_PORT_PIN
#define LED_BLUE_TOG()                 LED_BLUE_PORT->ODR ^= LED_BLUE_PORT_PIN

#define LED_GREEN_ON()                 LED_GREEN_PORT->BSRRH = LED_GREEN_PORT_PIN
#define LED_GREEN_OFF()                LED_GREEN_PORT->BSRRL = LED_GREEN_PORT_PIN
#define LED_GREEN_TOG()                LED_GREEN_PORT->ODR ^= LED_GREEN_PORT_PIN

void LED_Init(void);

#endif /* __LED_H */
