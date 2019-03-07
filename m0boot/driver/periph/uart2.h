/*
 * uart2.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef UART2_H_
#define UART2_H_

#include "SysConfig.h"

/* Not available on STM32F04x devices ??? */
#define UART2_DMA_ENABLE                    (0)

#define UART2                               USART2
#define UART2_GPIO                          GPIOA
#define UART2_AF                            GPIO_AF_1
#define UART2_GPIO_CLK                      RCC_AHBPeriph_GPIOA
#define UART2_CLK                           RCC_APB1Periph_USART2
#define UART2_CLK_CMD                       RCC_APB1PeriphClockCmd
#define UART2_TDR_Address                   0x40004428
#define UART2_RxPin                         GPIO_Pin_3
#define UART2_TxPin                         GPIO_Pin_2
#define UART2_RxPin_AF                      GPIO_PinSource3
#define UART2_TxPin_AF                      GPIO_PinSource2
#define UART2_IRQn                          USART2_IRQn
#define UART2_IRQHandler                    USART2_IRQHandler

#if UART2_DMA_ENABLE
#define UART2_DMA                           DMA1_Channel4
#define UART2_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART2_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART2_DMA_TC_FLAG                   DMA1_FLAG_TC4
#define UART2_DMA_IRQn                      11                         /*!< DMA1 Channel 4, Channel 5 Interrupts */
#define UART2_DMA_IRQHandler                DMA1_Channel4_5_IRQHandler
#endif /* UART2_DMA_ENABLE */

typedef void (*PortRecvByteCallback)(uint8_t Data);

void uart2_init(PortRecvByteCallback p);

void uart2_TxByte(uint8_t c);
void uart2_TxBytes(uint8_t *p, uint32_t l);
#if UART2_DMA_ENABLE
void uart2_TxBytesDMA(uint8_t *p, uint32_t l);
#else
#define uart2_TxBytesDMA uart2_TxBytes
#endif /* UART2_DMA_ENABLE */
void uart2_set_callback(PortRecvByteCallback p);

#endif /* UART2_H_ */