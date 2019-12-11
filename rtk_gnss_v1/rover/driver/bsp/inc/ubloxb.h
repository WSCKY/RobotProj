#ifndef __UBLOXB_H
#define __UBLOXB_H

#include "SysConfig.h"

/* Definition for UBXB_UART clock resources */
#define UBXB_UART                            USART6
#define UBXB_UART_CLK_ENABLE()               __USART6_CLK_ENABLE()
#define UBXB_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA2_CLK_ENABLE()
#define UBXB_UART_RX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()
#define UBXB_UART_TX_GPIO_CLK_ENABLE()       __GPIOC_CLK_ENABLE()

#define UBXB_UART_FORCE_RESET()              __USART6_FORCE_RESET()
#define UBXB_UART_RELEASE_RESET()            __USART6_RELEASE_RESET()

/* Definition for UBXB_UART Pins */
#define UBXB_UART_TX_PIN                     GPIO_PIN_6
#define UBXB_UART_TX_GPIO_PORT               GPIOC
#define UBXB_UART_TX_AF                      GPIO_AF8_USART6
#define UBXB_UART_RX_PIN                     GPIO_PIN_7
#define UBXB_UART_RX_GPIO_PORT               GPIOC
#define UBXB_UART_RX_AF                      GPIO_AF8_USART6

/* Definition for UBXB_UART's DMA */
#define UBXB_UART_TX_DMA_STREAM              DMA2_Stream6
#define UBXB_UART_RX_DMA_STREAM              DMA2_Stream2
#define UBXB_UART_TX_DMA_CHANNEL             DMA_CHANNEL_5
#define UBXB_UART_RX_DMA_CHANNEL             DMA_CHANNEL_5

/* Definition for UBXB_UART's NVIC */
#define UBXB_UART_DMA_TX_IRQn                DMA2_Stream6_IRQn
#define UBXB_UART_DMA_RX_IRQn                DMA2_Stream2_IRQn
#define UBXB_UART_DMA_TX_IRQHandler          DMA2_Stream6_IRQHandler
#define UBXB_UART_DMA_RX_IRQHandler          DMA2_Stream2_IRQHandler

/* Definition for UBXB_UART's NVIC */
#define UBXB_UART_IRQn                       USART6_IRQn
#define UBXB_UART_IRQHandler                 USART6_IRQHandler

#define UBXBIF_CACHE_DEPTH                   (256)

status_t ubxbif_init(void);
void ubxbif_reset_enable(void);
void ubxbif_reset_disable(void);
status_t ubxbif_set_baudrate(uint32_t baudrate);
uint32_t ubxbif_rx_bytes(uint8_t *p, uint32_t l);
status_t ubxbif_tx_bytes(uint8_t *p, uint32_t l);
status_t ubxbif_tx_bytes_util(uint8_t *p, uint32_t l);

void ubxbif_msp_init(UART_HandleTypeDef *huart);
void ubxbif_msp_deinit(UART_HandleTypeDef *huart);
void ubxbif_txcplt_callback(UART_HandleTypeDef *huart);
void ubxbif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __UBLOXB_H */
