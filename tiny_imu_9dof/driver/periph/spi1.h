/*
 * spi1.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef DRIVER_PERIPH_SPI1_H_
#define DRIVER_PERIPH_SPI1_H_

#include "SysConfig.h"

#define SPI1_DMA_ENABLE                     (1)

/* Communication boards SPIx Interface */
#define SPI1_SCK_PIN                     GPIO_Pin_5                  /* PA.05 */
#define SPI1_SCK_GPIO_PORT               GPIOA                       /* GPIOA */
#define SPI1_SCK_GPIO_CLK                RCC_AHBPeriph_GPIOB
#define SPI1_SCK_SOURCE                  GPIO_PinSource5
#define SPI1_SCK_AF                      GPIO_AF_0

#define SPI1_MISO_PIN                    GPIO_Pin_6                  /* PA.06 */
#define SPI1_MISO_GPIO_PORT              GPIOA                       /* GPIOA */
#define SPI1_MISO_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SPI1_MISO_SOURCE                 GPIO_PinSource6
#define SPI1_MISO_AF                     GPIO_AF_0

#define SPI1_MOSI_PIN                    GPIO_Pin_7                  /* PA.07 */
#define SPI1_MOSI_GPIO_PORT              GPIOA                       /* GPIOA */
#define SPI1_MOSI_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define SPI1_MOSI_SOURCE                 GPIO_PinSource7
#define SPI1_MOSI_AF                     GPIO_AF_0

#define SPI1_NSS_PIN                     GPIO_Pin_4                  /* PA.04 */
#define SPI1_NSS_GPIO_PORT               GPIOA
#define SPI1_NSS_GPIO_CLK                RCC_AHBPeriph_GPIOA

#if SPI1_DMA_ENABLE
#define SPI1_RX_DMA                      DMA1_Channel2
#define SPI1_TX_DMA                      DMA1_Channel3
#define SPI1_DMA_CLK                     RCC_AHBPeriph_DMA1
#define SPI1_DMA_CLK_CMD                 RCC_AHBPeriphClockCmd
#define SPI1_RX_DMA_IT_TC_FLAG           DMA1_IT_TC2
#define SPI1_TX_DMA_IT_TC_FLAG           DMA1_IT_TC3
#define SPI1_DMA_IRQn                    DMA1_Channel2_3_IRQn
#define SPI1_DMA_IRQHandler              DMA1_Channel2_3_IRQHandler
#endif /* SPI1_DMA_ENABLE */

#define SPI1_NSS_ENABLE()                (SPI1_NSS_GPIO_PORT->BRR = SPI1_NSS_PIN)
#define SPI1_NSS_DISABLE()               (SPI1_NSS_GPIO_PORT->BSRR = SPI1_NSS_PIN)

#define SPI1_DATASIZE                    SPI_DataSize_8b
#define SPI1_DATAMASK                    (uint8_t)0xFF

void spi1_init(void);
void spi1_configrate(uint32_t scalingfactor);
void spi1_rx_tx(uint8_t *w, uint8_t *r, uint16_t l);
#if SPI1_DMA_ENABLE
void spi1_rx_tx_dma(uint8_t *w, uint8_t *r, uint16_t l);
void spi1_rx_tx_dma_util(uint8_t *w, uint8_t *r, uint16_t l);
#else
#define spi1_rx_tx_dma spi1_rx_tx
#define spi1_rx_tx_dma_util spi1_rx_tx
#endif /* SPI1_DMA_ENABLE */

#endif /* DRIVER_PERIPH_SPI1_H_ */
