/*
 * pwm.h
 *
 *  Created on: Mar 9, 2019
 *      Author: kychu
 */

#ifndef PERIPH_PWM_H_
#define PERIPH_PWM_H_

#include "SysConfig.h"

void pwm_init(uint8_t d);
void pwm_set_dutycycle(uint8_t d);

#endif /* PERIPH_PWM_H_ */
