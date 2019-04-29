#ifndef __BUTTON_H
#define __BUTTON_H

#include "SysConfig.h"

#define USER_BUTTON_PIN                GPIO_Pin_15
#define USER_BUTTON_GPIO_PORT          GPIOA
#define USER_BUTTON_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define USER_BUTTON_EXTI_LINE          EXTI_Line15
#define USER_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOA
#define USER_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource15
#define USER_BUTTON_EXTI_IRQn          EXTI15_10_IRQn

#define USER_BUTTON_GET_STATE()        ((USER_BUTTON_GPIO_PORT->IDR & USER_BUTTON_PIN) ? Bit_SET : Bit_RESET)

void button_init(void);
void button_int_isr(void);
void button_press_callback(void) __attribute__((weak));

#endif /* __BUTTON_H */
