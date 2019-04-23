#ifndef __UBXUART_H
#define __UBXUART_H

#include "SysConfig.h"

/* Definition for USARTx resources ******************************************/
#define UBX_PORT                       USART1
#define UBX_PORT_CLK                   RCC_APB2Periph_USART1
#define UBX_PORT_CLK_INIT              RCC_APB2PeriphClockCmd
#define UBX_PORT_TX_PIN                GPIO_Pin_6
#define UBX_PORT_TX_GPIO_PORT          GPIOB
#define UBX_PORT_TX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define UBX_PORT_TX_SOURCE             GPIO_PinSource6
#define UBX_PORT_TX_AF                 GPIO_AF_USART1
#define UBX_PORT_RX_PIN                GPIO_Pin_7
#define UBX_PORT_RX_GPIO_PORT          GPIOB
#define UBX_PORT_RX_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define UBX_PORT_RX_SOURCE             GPIO_PinSource7
#define UBX_PORT_RX_AF                 GPIO_AF_USART1
#define UBX_PORT_IRQn                  USART1_IRQn
#define UBX_PORT_IRQHandler            USART1_IRQHandler

/* Definition for DMAx resources ********************************************/
#define UBX_PORT_DR_ADDRESS            ((uint32_t)USART1 + 0x04)

#define UBX_PORT_DMA                   DMA2
#define UBX_PORT_DMAx_CLK              RCC_AHB1Periph_DMA2
#define UBX_PORT_DMAx_CLK_INIT         RCC_AHB1PeriphClockCmd

#define UBX_PORT_TX_DMA_CHANNEL        DMA_Channel_4
#define UBX_PORT_TX_DMA_STREAM         DMA2_Stream7
#define UBX_PORT_TX_DMA_FLAG_FEIF      DMA_FLAG_FEIF7
#define UBX_PORT_TX_DMA_FLAG_DMEIF     DMA_FLAG_DMEIF7
#define UBX_PORT_TX_DMA_FLAG_TEIF      DMA_FLAG_TEIF7
#define UBX_PORT_TX_DMA_FLAG_HTIF      DMA_FLAG_HTIF7
#define UBX_PORT_TX_DMA_FLAG_TCIF      DMA_FLAG_TCIF7

//#define UBX_PORT_RX_DMA_CHANNEL        DMA_Channel_4
//#define UBX_PORT_RX_DMA_STREAM         DMA2_Stream5
//#define UBX_PORT_RX_DMA_FLAG_FEIF      DMA_FLAG_FEIF5
//#define UBX_PORT_RX_DMA_FLAG_DMEIF     DMA_FLAG_DMEIF5
//#define UBX_PORT_RX_DMA_FLAG_TEIF      DMA_FLAG_TEIF5
//#define UBX_PORT_RX_DMA_FLAG_HTIF      DMA_FLAG_HTIF5
//#define UBX_PORT_RX_DMA_FLAG_TCIF      DMA_FLAG_TCIF5

#define UBX_PORT_DMA_TX_IRQn           DMA2_Stream7_IRQn
//#define UBX_PORT_DMA_RX_IRQn           DMA2_Stream5_IRQn
#define UBX_PORT_DMA_TX_IRQHandler     DMA2_Stream7_IRQHandler
//#define UBX_PORT_DMA_RX_IRQHandler     DMA2_Stream5_IRQHandler

typedef enum {
  RX_DISABLE = 0,
  RX_ENABLE = 1,
} RX_STATUS;

void ubx_port_init(void);
void ubxPortRxDataCallback(uint8_t Data);
void ubxPortSendBytes(uint8_t *p, uint32_t l);
void ubxPortSendBytesDMA(uint8_t *p, uint32_t l);
void ubx_port_config(uint32_t BaudRate, RX_STATUS RxEnable);

#endif /* __UBXUART_H */
