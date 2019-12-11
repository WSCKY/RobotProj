#ifndef __EC20_H
#define __EC20_H

#include "SysConfig.h"

#define EC20_Delay           osDelay
#define EC20_Ticks           osKernelSysTick

/* Definition for EC20_UART clock resources */
#define EC20_UART                            UART4
#define EC20_UART_CLK_ENABLE()               __UART4_CLK_ENABLE()
#define EC20_UART_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()
#define EC20_UART_RX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()
#define EC20_UART_TX_GPIO_CLK_ENABLE()       __GPIOA_CLK_ENABLE()

#define EC20_UART_FORCE_RESET()              __UART4_FORCE_RESET()
#define EC20_UART_RELEASE_RESET()            __UART4_RELEASE_RESET()

/* Definition for EC20_UART Pins */
#define EC20_UART_TX_PIN                     GPIO_PIN_0
#define EC20_UART_TX_GPIO_PORT               GPIOA
#define EC20_UART_TX_AF                      GPIO_AF8_UART4
#define EC20_UART_RX_PIN                     GPIO_PIN_1
#define EC20_UART_RX_GPIO_PORT               GPIOA
#define EC20_UART_RX_AF                      GPIO_AF8_UART4

/* Definition for EC20_UART's DMA */
#define EC20_UART_TX_DMA_STREAM              DMA1_Stream4
#define EC20_UART_RX_DMA_STREAM              DMA1_Stream2
#define EC20_UART_TX_DMA_CHANNEL             DMA_CHANNEL_4
#define EC20_UART_RX_DMA_CHANNEL             DMA_CHANNEL_4

/* Definition for EC20_UART's NVIC */
#define EC20_UART_DMA_TX_IRQn                DMA1_Stream4_IRQn
#define EC20_UART_DMA_RX_IRQn                DMA1_Stream2_IRQn
#define EC20_UART_DMA_TX_IRQHandler          DMA1_Stream4_IRQHandler
#define EC20_UART_DMA_RX_IRQHandler          DMA1_Stream2_IRQHandler

/* Definition for EC20_UART's NVIC */
#define EC20_UART_IRQn                       UART4_IRQn
#define EC20_UART_IRQHandler                 UART4_IRQHandler

#define EC20IF_CACHE_DEPTH                   (768)

status_t ec20if_init(void);
uint32_t ec20if_cache_usage(void);
uint32_t ec20if_rx_bytes(uint8_t *p, uint32_t l);
status_t ec20if_tx_bytes(uint8_t *p, uint32_t l);
status_t ec20if_tx_bytes_util(uint8_t *p, uint32_t l);
status_t ec20if_tx_string_util(const char *str);

/*
timeout in ms.
*/
status_t ec20_check_ack(uint8_t *p, uint32_t timeout);
status_t ec20_check_cmd_ack(uint8_t *cmd, uint8_t *ack, uint32_t tries, uint32_t timeout);

void ec20if_msp_init(UART_HandleTypeDef *huart);
void ec20if_msp_deinit(UART_HandleTypeDef *huart);
void ec20if_txcplt_callback(UART_HandleTypeDef *huart);
void ec20if_rxcplt_callback(UART_HandleTypeDef *huart);

void ec20_power_init(void);
void ec20_power_on(void);
void ec20_power_off(void);

#endif /* __EC20_H */
