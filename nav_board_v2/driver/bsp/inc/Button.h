#ifndef __BUTTON_H
#define __BUTTON_H

#include "SysConfig.h"

#define USER_BUTTON_PIN                GPIO_Pin_15
#define USER_BUTTON_GPIO_PORT          GPIOB
#define USER_BUTTON_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define USER_BUTTON_EXTI_LINE          EXTI_Line15
#define USER_BUTTON_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOB
#define USER_BUTTON_EXTI_PIN_SOURCE    EXTI_PinSource15
#define USER_BUTTON_EXTI_IRQn          EXTI15_10_IRQn

#define USER_BUTTON_GET_STATE()        ((USER_BUTTON_GPIO_PORT->IDR & USER_BUTTON_PIN) ? Bit_SET : Bit_RESET)

#if FREERTOS_ENABLED
  osSemaphoreId ButtonInit(void);
  osSemaphoreId GetButtonSemaphore(void);
#else
  void ButtonInit(void);
#endif /* FREERTOS_ENABLED */

void button_int_isr(void);

#endif /* __BUTTON_H */
