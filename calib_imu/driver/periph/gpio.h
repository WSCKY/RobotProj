/*
 * gpio.h
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#ifndef PERIPH_GPIO_H_
#define PERIPH_GPIO_H_

#include "SysConfig.h"

void user_io_init(void);

#define USER_IO_LOW()                  GPIOB->BRR = GPIO_Pin_1;
#define USER_IO_HIGH()                 GPIOB->BSRR = GPIO_Pin_1;
#define USER_IO_TOGGLE()               GPIOB->ODR ^= GPIO_Pin_1;

#endif /* PERIPH_GPIO_H_ */
