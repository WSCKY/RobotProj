/*
 * gpio.h
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#ifndef DRIVER_BSP_INC_GPIO_H_
#define DRIVER_BSP_INC_GPIO_H_

#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H

#include "SysConfig.h"

#define IMU_DEN_GPIO_PORT                 GPIOB
#define IMU_DEN_GPIO_PORT_PIN             GPIO_Pin_9
#define IMU_DEN_GPIO_PORT_CLK             RCC_AHB1Periph_GPIOB

void gpio_init(void);

#endif /* __BSP_GPIO_H */

#endif /* DRIVER_BSP_INC_GPIO_H_ */
