/*
 * ifconfig.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef DRIVER_DEVICE_IFCONFIG_H_
#define DRIVER_DEVICE_IFCONFIG_H_

#include "i2c1.h"
#include "spi1.h"
#include "intio.h"
#include "uart2.h"
#include "pwm.h"
#include "gpio.h"

#define USER_UART_TX_BYTE                   uart2_TxByte
#define USER_UART_TX_BYTES                  uart2_TxBytesDMA

#define USER_PWM_SET_DUTYCYCLE              pwm_set_dutycycle

#define USER_LED_ON                         USER_IO_LOW
#define USER_LED_OFF                        USER_IO_HIGH
#define USER_LED_TOG                        USER_IO_TOGGLE

#endif /* DRIVER_DEVICE_IFCONFIG_H_ */
