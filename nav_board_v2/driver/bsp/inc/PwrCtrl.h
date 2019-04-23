#ifndef __PWRCTRL_H
#define __PWRCTRL_H

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#define PWR_3V3_PORT         GPIOA
#define PWR_3V3_PORT_PIN     GPIO_Pin_9

#define PWR_IMU_PORT         GPIOB
#define PWR_IMU_PORT_PIN     GPIO_Pin_2

#define PWR_3V3_ON()         PWR_3V3_PORT->BSRRH = PWR_3V3_PORT_PIN
#define PWR_3V3_OFF()        PWR_3V3_PORT->BSRRL = PWR_3V3_PORT_PIN

#define PWR_IMU_ON()         PWR_IMU_PORT->BSRRL = PWR_IMU_PORT_PIN
#define PWR_IMU_OFF()        PWR_IMU_PORT->BSRRH = PWR_IMU_PORT_PIN

void PWR_CTRL_Init(void);

#endif /* __PWRCTRL_H */
