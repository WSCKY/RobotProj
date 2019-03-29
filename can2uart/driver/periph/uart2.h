/*
 * uart2.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef UART2_H_
#define UART2_H_

#include "SysConfig.h"

#define UART2_DMA_ENABLE                    (1)

#define UART2                               USART2
#define UART2_GPIO                          GPIOA
#define UART2_AF                            GPIO_AF_1
#define UART2_GPIO_CLK                      RCC_AHBPeriph_GPIOA
#define UART2_CLK                           RCC_APB1Periph_USART2
#define UART2_CLK_CMD                       RCC_APB1PeriphClockCmd
#define UART2_RxPin                         GPIO_Pin_3
#define UART2_TxPin                         GPIO_Pin_2
#define UART2_RxPin_AF                      GPIO_PinSource3
#define UART2_TxPin_AF                      GPIO_PinSource2
#define UART2_IRQn                          USART2_IRQn
#define UART2_IRQHandler                    USART2_IRQHandler

#if UART2_DMA_ENABLE
#define UART2_TX_DMA                        DMA1_Channel4
#define UART2_RX_DMA                        DMA1_Channel5
#define UART2_DMA_CLK                       RCC_AHBPeriph_DMA1
#define UART2_DMA_CLK_CMD                   RCC_AHBPeriphClockCmd
#define UART2_TX_DMA_IT_TC_FLAG             DMA1_IT_TC4
#define UART2_RX_DMA_IT_HT_FLAG             DMA1_IT_HT5
#define UART2_RX_DMA_IT_TC_FLAG             DMA1_IT_TC5
#define UART2_DMA_IRQn                      DMA1_Channel4_5_IRQn   /*!< DMA1 Channel 4, Channel 5 Interrupts */
#define UART2_DMA_IRQHandler                DMA1_Channel4_5_IRQHandler
#endif /* UART2_DMA_ENABLE */

typedef void (*PortRecvByteCallback)(uint8_t Data);

void uart2_init(
#if UART2_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif
		);

void uart2_TxByte(uint8_t c);
void uart2_TxBytes(uint8_t *p, uint32_t l);
#if UART2_DMA_ENABLE
void uart2_flush(void);
void uart2_TxBytesDMA(uint8_t *p, uint32_t l);
uint8_t uart2_pullByte(uint8_t *p);
uint32_t uart2_pullBytes(uint8_t *p, uint32_t l);
#else
#define uart2_TxBytesDMA uart2_TxBytes
void uart2_set_callback(PortRecvByteCallback p);
#endif /* UART2_DMA_ENABLE */

#endif /* UART2_H_ */
