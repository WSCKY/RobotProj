#ifndef __RTCMIF_H
#define __RTCMIF_H

#include "SysConfig.h"

/* Definition for RTCM_UART clock resources */
#define RTCM_UART                            USART3
#define RTCM_UART_CLK_ENABLE()               __USART3_CLK_ENABLE()
#define RTCM_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
#define RTCM_UART_RX_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()
#define RTCM_UART_TX_GPIO_CLK_ENABLE()       __GPIOB_CLK_ENABLE()

#define RTCM_UART_FORCE_RESET()              __USART3_FORCE_RESET()
#define RTCM_UART_RELEASE_RESET()            __USART3_RELEASE_RESET()

/* Definition for RTCM_UART Pins */
#define RTCM_UART_TX_PIN                     GPIO_PIN_10
#define RTCM_UART_TX_GPIO_PORT               GPIOB
#define RTCM_UART_TX_AF                      GPIO_AF7_USART3
#define RTCM_UART_RX_PIN                     GPIO_PIN_11
#define RTCM_UART_RX_GPIO_PORT               GPIOB
#define RTCM_UART_RX_AF                      GPIO_AF7_USART3

/* Definition for RTCM_UART's DMA */
#define RTCM_UART_TX_DMA_STREAM              DMA1_Stream3
#define RTCM_UART_RX_DMA_STREAM              DMA1_Stream1
#define RTCM_UART_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define RTCM_UART_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for RTCM_UART's NVIC */
#define RTCM_UART_DMA_TX_IRQn                DMA1_Stream3_IRQn
#define RTCM_UART_DMA_RX_IRQn                DMA1_Stream1_IRQn
#define RTCM_UART_DMA_TX_IRQHandler          DMA1_Stream3_IRQHandler
#define RTCM_UART_DMA_RX_IRQHandler          DMA1_Stream1_IRQHandler

/* Definition for RTCM_UART's NVIC */
#define RTCM_UART_IRQn                       USART3_IRQn
#define RTCM_UART_IRQHandler                 USART3_IRQHandler

#define RTCM_CACHE_DEPTH                     (512)

status_t rtcmif_init(void);
uint32_t rtcmif_cache_usage(void);
uint32_t rtcmif_rx_bytes(uint8_t *p, uint32_t l);
status_t rtcmif_tx_bytes(uint8_t *p, uint32_t l);
status_t rtcmif_tx_bytes_util(uint8_t *p, uint32_t l);

void rtcmif_msp_init(UART_HandleTypeDef *huart);
void rtcmif_msp_deinit(UART_HandleTypeDef *huart);
void rtcmif_txcplt_callback(UART_HandleTypeDef *huart);
void rtcmif_rxcplt_callback(UART_HandleTypeDef *huart);

#endif /* __RTCMIF_H */
