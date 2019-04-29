#ifndef __COLOR_LED_H
#define __COLOR_LED_H

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define LED_R_PORT                   GPIOB
#define LED_R_PORT_PIN               GPIO_Pin_3
#define LED_R_PORT_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define LED_G_PORT                   GPIOB
#define LED_G_PORT_PIN               GPIO_Pin_4
#define LED_G_PORT_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define LED_B_PORT                   GPIOB
#define LED_B_PORT_PIN               GPIO_Pin_5
#define LED_B_PORT_GPIO_CLK          RCC_AHB1Periph_GPIOB

#define LED_R_ON()                   LED_R_PORT->BSRRH = LED_R_PORT_PIN
#define LED_R_OFF()                  LED_R_PORT->BSRRL = LED_R_PORT_PIN
#define LED_R_TOG()                  LED_R_PORT->ODR ^= LED_R_PORT_PIN

#define LED_G_ON()                   LED_G_PORT->BSRRH = LED_G_PORT_PIN
#define LED_G_OFF()                  LED_G_PORT->BSRRL = LED_G_PORT_PIN
#define LED_G_TOG()                  LED_G_PORT->ODR ^= LED_G_PORT_PIN

#define LED_B_ON()                   LED_B_PORT->BSRRH = LED_B_PORT_PIN
#define LED_B_OFF()                  LED_B_PORT->BSRRL = LED_B_PORT_PIN
#define LED_B_TOG()                  LED_B_PORT->ODR ^= LED_B_PORT_PIN

void color_led_init(void);

#endif /* __COLOR_LED_H */
