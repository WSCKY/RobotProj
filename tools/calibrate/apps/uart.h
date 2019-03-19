/*
 * uart.h
 *
 *  Created on: Feb 18, 2019
 *      Author: kychu
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>

typedef void (*UartRecvByteCallback)(unsigned char Data);

int uart_open(const char *dev, char *baud);
int uart_write(char *p, size_t l);
int uart_read(char *p, size_t l);
void uart_close(void);
int uart_isopen(void);
int uart_auto_recv_enable(void);
void uart_set_recv_callback(UartRecvByteCallback p);


#endif /* UART_H_ */
