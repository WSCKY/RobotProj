/*
 * flashif.h
 *
 *  Created on: Dec 24, 2019
 *      Author: kychu
 */

#ifndef BASE_DRIVER_BSP_INC_FLASHIF_H_
#define BASE_DRIVER_BSP_INC_FLASHIF_H_

#include "SysConfig.h"

#define FLASH_SPI                             SPI2
#define FLASH_SPI_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define FLASH_SPI_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLASH_SPI_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLASH_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define FLASH_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define FLASH_SPI_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define FLASH_SPI_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for FLASH_SPI Pins */
#define FLASH_SPI_SCK_PIN                     GPIO_PIN_13
#define FLASH_SPI_SCK_GPIO_PORT               GPIOB
#define FLASH_SPI_SCK_AF                      GPIO_AF5_SPI2
#define FLASH_SPI_MISO_PIN                    GPIO_PIN_14
#define FLASH_SPI_MISO_GPIO_PORT              GPIOB
#define FLASH_SPI_MISO_AF                     GPIO_AF5_SPI2
#define FLASH_SPI_MOSI_PIN                    GPIO_PIN_15
#define FLASH_SPI_MOSI_GPIO_PORT              GPIOB
#define FLASH_SPI_MOSI_AF                     GPIO_AF5_SPI2

#define FLASH_SPI_NSS_PIN                     GPIO_PIN_12
#define FLASH_SPI_NSS_GPIO_PORT               GPIOB

#define FLASH_WP_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define FLASH_WP_PIN                          GPIO_PIN_8
#define FLASH_WP_GPIO_PORT                    GPIOC

/* Definition for FLASH_SPI's NVIC */
#define FLASH_SPI_IRQn                        SPI2_IRQn
#define FLASH_SPI_IRQHandler                  SPI2_IRQHandler

status_t flashif_init(void);
status_t flashif_select(uint8_t id);
status_t flashif_deselect(uint8_t id);
status_t flashif_tx_bytes(uint8_t *pTxData, uint16_t Size);
status_t flashif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
status_t flashif_txrx_bytes_it(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

void flashif_msp_init(SPI_HandleTypeDef *hspi);
void flashif_msp_deinit(SPI_HandleTypeDef *hspi);
void flashif_rxtxcplt_callback(SPI_HandleTypeDef *hspi);

#endif /* BASE_DRIVER_BSP_INC_FLASHIF_H_ */
