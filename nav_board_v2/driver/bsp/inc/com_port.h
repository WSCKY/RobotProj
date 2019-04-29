#ifndef __COMPORT_H
#define __COMPORT_H

#include "SysConfig.h"

#define COM_PORT_DMA_ENABLE            (1)

/* Definition for USARTx resources ******************************************/
#define COM_PORT                       USART2
#define COM_PORT_CLK                   RCC_APB1Periph_USART2
#define COM_PORT_CLK_INIT              RCC_APB1PeriphClockCmd
#define COM_PORT_TX_PIN                GPIO_Pin_2
#define COM_PORT_TX_GPIO_PORT          GPIOA
#define COM_PORT_TX_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define COM_PORT_TX_SOURCE             GPIO_PinSource2
#define COM_PORT_TX_AF                 GPIO_AF_USART2
#define COM_PORT_RX_PIN                GPIO_Pin_3
#define COM_PORT_RX_GPIO_PORT          GPIOA
#define COM_PORT_RX_GPIO_CLK           RCC_AHB1Periph_GPIOA
#define COM_PORT_RX_SOURCE             GPIO_PinSource3
#define COM_PORT_RX_AF                 GPIO_AF_USART2
#define COM_PORT_IRQn                  USART2_IRQn
#define COM_PORT_IRQHandler            USART2_IRQHandler

/* Definition for DMAx resources ********************************************/
#define COM_PORT_DR_ADDRESS            ((uint32_t)USART2 + 0x04) 

#if COM_PORT_DMA_ENABLE
#define COM_PORT_DMA                   DMA1
#define COM_PORT_DMAx_CLK              RCC_AHB1Periph_DMA1
#define COM_PORT_DMAx_CLK_INIT         RCC_AHB1PeriphClockCmd

#define COM_PORT_TX_DMA_CHANNEL        DMA_Channel_4
#define COM_PORT_TX_DMA_STREAM         DMA1_Stream6
#define COM_PORT_TX_DMA_FLAG_FEIF      DMA_FLAG_FEIF6
#define COM_PORT_TX_DMA_FLAG_DMEIF     DMA_FLAG_DMEIF6
#define COM_PORT_TX_DMA_FLAG_TEIF      DMA_FLAG_TEIF6
#define COM_PORT_TX_DMA_FLAG_HTIF      DMA_FLAG_HTIF6
#define COM_PORT_TX_DMA_FLAG_TCIF      DMA_FLAG_TCIF6
#define COM_PORT_TX_DMA_IT_TCIF        DMA_IT_TCIF6

#define COM_PORT_RX_DMA_CHANNEL        DMA_Channel_4
#define COM_PORT_RX_DMA_STREAM         DMA1_Stream5
#define COM_PORT_RX_DMA_FLAG_FEIF      DMA_FLAG_FEIF5
#define COM_PORT_RX_DMA_FLAG_DMEIF     DMA_FLAG_DMEIF5
#define COM_PORT_RX_DMA_FLAG_TEIF      DMA_FLAG_TEIF5
#define COM_PORT_RX_DMA_FLAG_HTIF      DMA_FLAG_HTIF5
#define COM_PORT_RX_DMA_FLAG_TCIF      DMA_FLAG_TCIF5
#define COM_PORT_RX_DMA_IT_TCIF        DMA_IT_TCIF5
#define COM_PORT_RX_DMA_IT_HTIF        DMA_IT_HTIF5

//#define COM_PORT_DMA_TX_IRQn           DMA1_Stream6_IRQn
//#define COM_PORT_DMA_RX_IRQn           DMA1_Stream5_IRQn
//#define COM_PORT_DMA_TX_IRQHandler     DMA1_Stream6_IRQHandler
//#define COM_PORT_DMA_RX_IRQHandler     DMA1_Stream5_IRQHandler
#endif /* COM_PORT_DMA_ENABLE */

void com_port_init(
#if COM_PORT_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif /* COM_PORT_DMA_ENABLE */
		);

void ComPort_TxByte(uint8_t c);
void ComPort_TxBytes(uint8_t *p, uint32_t l);
#if COM_PORT_DMA_ENABLE
void ComPort_flush(void);
void ComPort_TxBytesDMA(uint8_t *p, uint32_t l);
uint8_t ComPort_pullByte(uint8_t *p);
uint32_t ComPort_pullBytes(uint8_t *p, uint32_t l);
#else
#define ComPort_TxBytesDMA ComPort_TxBytes
void ComPort_set_callback(PortRecvByteCallback p);
#endif /* COM_PORT_DMA_ENABLE */

void com_port_dma_tx_isr(void);
void com_port_dma_rx_isr(void);
#endif /* __COMPORT_H */
