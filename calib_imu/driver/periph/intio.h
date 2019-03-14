/*
 * intio.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef DRIVER_PERIPH_INTIO_H_
#define DRIVER_PERIPH_INTIO_H_

#include "SysConfig.h"

#define INTx_PIN                            GPIO_Pin_0                  /* PB.00 */
#define INTx_GPIO_PORT                      GPIOB
#define INTx_GPIO_CLK                       RCC_AHBPeriph_GPIOB
#define INTx_GPIO_PortSource                EXTI_PortSourceGPIOB
#define INTx_GPIO_PinSource                 EXTI_PinSource0
#define INTx_EXTI_LINE                      EXTI_Line0
#define INTx_EXTI_IRQn                      EXTI0_1_IRQn
#define INTx_EXTI_IRQHandler                EXTI0_1_IRQHandler

typedef void (*INTIO_IRQCallback)(void);

void intio_init(void);
void intio_set_irq_handler(INTIO_IRQCallback p);

#endif /* DRIVER_PERIPH_INTIO_H_ */
