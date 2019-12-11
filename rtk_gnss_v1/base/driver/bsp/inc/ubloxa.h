#ifndef __UBLOXA_H
#define __UBLOXA_H

#include "SysConfig.h"

/* Definition for UBXA_UART clock resources */
#define UBXA_UART                            USART1
#define UBXA_UART_CLK_ENABLE()               __USART1_CLK_ENABLE()
#define UBXA_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
#define UBXA_UART_RX_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define UBXA_UART_TX_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()

#define UBXA_UART_FORCE_RESET()              __USART1_FORCE_RESET()
#define UBXA_UART_RELEASE_RESET()            __USART1_RELEASE_RESET()

/* Definition for UBXA_UART Pins */
#define UBXA_UART_TX_PIN                     GPIO_PIN_6
#define UBXA_UART_TX_GPIO_PORT               GPIOB
#define UBXA_UART_TX_AF                      GPIO_AF7_USART1
#define UBXA_UART_RX_PIN                     GPIO_PIN_7
#define UBXA_UART_RX_GPIO_PORT               GPIOB
#define UBXA_UART_RX_AF                      GPIO_AF7_USART1

/* Definition for UBXA_UART's DMA */
#define UBXA_UART_TX_DMA_STREAM              DMA2_Stream7
#define UBXA_UART_RX_DMA_STREAM              DMA2_Stream5
#define UBXA_UART_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define UBXA_UART_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for UBXA_UART's NVIC */
#define UBXA_UART_DMA_TX_IRQn                DMA2_Stream7_IRQn
#define UBXA_UART_DMA_RX_IRQn                DMA2_Stream5_IRQn
#define UBXA_UART_DMA_TX_IRQHandler          DMA2_Stream7_IRQHandler
#define UBXA_UART_DMA_RX_IRQHandler          DMA2_Stream5_IRQHandler

/* Definition for UBXA_UART's NVIC */
#define UBXA_UART_IRQn                       USART1_IRQn
#define UBXA_UART_IRQHandler                 USART1_IRQHandler

#define UBXAIF_CACHE_DEPTH                   (256)

status_t ubxaif_init(void);
void ubxaif_reset_enable(void);
void ubxaif_reset_disable(void);
status_t ubxaif_set_baudrate(uint32_t baudrate);
uint32_t ubxaif_rx_bytes(uint8_t *p, uint32_t l);
status_t ubxaif_tx_bytes(uint8_t *p, uint32_t l);
status_t ubxaif_tx_bytes_util(uint8_t *p, uint32_t l);

void ubxaif_msp_init(UART_HandleTypeDef *huart);
void ubxaif_msp_deinit(UART_HandleTypeDef *huart);
void ubxaif_txcplt_callback(UART_HandleTypeDef *huart);
void ubxaif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __UBLOXA_H */
