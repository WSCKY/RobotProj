/*
 * imuif.h
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#ifndef BASE_DRIVER_BSP_INC_IMUIF_H_
#define BASE_DRIVER_BSP_INC_IMUIF_H_

#include "SysConfig.h"

#define IMU_SPI                             SPI1
#define IMU_SPI_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define IMU_SPI_DMA_CLK_ENABLE()            __HAL_RCC_DMA2_CLK_ENABLE()
#define IMU_SPI_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define IMU_SPI_NSS_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define IMU_SPI_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define IMU_SPI_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define IMU_SPI_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define IMU_SPI_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for IMU_SPI Pins */
#define IMU_SPI_SCK_PIN                     GPIO_PIN_5
#define IMU_SPI_SCK_GPIO_PORT               GPIOA
#define IMU_SPI_SCK_AF                      GPIO_AF5_SPI1
#define IMU_SPI_MISO_PIN                    GPIO_PIN_6
#define IMU_SPI_MISO_GPIO_PORT              GPIOA
#define IMU_SPI_MISO_AF                     GPIO_AF5_SPI1
#define IMU_SPI_MOSI_PIN                    GPIO_PIN_7
#define IMU_SPI_MOSI_GPIO_PORT              GPIOA
#define IMU_SPI_MOSI_AF                     GPIO_AF5_SPI1

#define IMU_SPI_NSS_PIN                     GPIO_PIN_4
#define IMU_SPI_NSS_GPIO_PORT               GPIOA

/* Definition for IMU_SPI's DMA */
#define IMU_SPI_TX_DMA_STREAM               DMA2_Stream3
#define IMU_SPI_RX_DMA_STREAM               DMA2_Stream0
#define IMU_SPI_TX_DMA_CHANNEL              DMA_CHANNEL_3
#define IMU_SPI_RX_DMA_CHANNEL              DMA_CHANNEL_3

/* Definition for IMU_SPI's NVIC */
#define IMU_SPI_DMA_TX_IRQn                 DMA2_Stream3_IRQn
#define IMU_SPI_DMA_RX_IRQn                 DMA2_Stream0_IRQn

#define IMU_SPI_DMA_TX_IRQHandler           DMA2_Stream3_IRQHandler
#define IMU_SPI_DMA_RX_IRQHandler           DMA2_Stream0_IRQHandler

#define IMU_INT1_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
#define IMU_INT2_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()

#define IMU_INT1_PIN                        GPIO_PIN_4
#define IMU_INT1_GPIO_PORT                  GPIOC
#define IMU_INT1_GPIO_EXTI_IRQn             EXTI4_IRQn

#define IMU_INT2_PIN                        GPIO_PIN_0
#define IMU_INT2_GPIO_PORT                  GPIOB
#define IMU_INT2_GPIO_EXTI_IRQn             EXTI0_IRQn

status_t imuif_init(void);
status_t imuif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);
status_t imuif_txrx_bytes_dma(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size);

void imuif_msp_init(SPI_HandleTypeDef *hspi);
void imuif_msp_deinit(SPI_HandleTypeDef *hspi);
void imuif_rxtxcplt_callback(SPI_HandleTypeDef *hspi);

void imuif_int1_callback(void);
void imuif_int2_callback(void);

#endif /* BASE_DRIVER_BSP_INC_IMUIF_H_ */
