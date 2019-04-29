#ifndef __W25Q_SPI_H
#define __W25Q_SPI_H

#include "SysConfig.h"

/* W25Q FLASH SPI Interface pins  */
#define W25Q_SPI                           SPI1
#define W25Q_SPI_CLK                       RCC_APB2Periph_SPI1
#define W25Q_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define W25Q_SPI_DMA_CLK                   RCC_AHB1Periph_DMA2
#define W25Q_SPI_DMA_CHANNEL               DMA_Channel_3
#define W25Q_SPI_DMA_RX_STREAM             DMA2_Stream2
#define W25Q_SPI_DMA_TX_STREAM             DMA2_Stream3
#define W25Q_SPI_DMA_RX_TCIF               DMA_IT_TCIF2
#define W25Q_SPI_DMA_TX_TCIF               DMA_IT_TCIF3
#define W25Q_SPI_DMA_RX_STREAM_IRQn        DMA2_Stream2_IRQn
#define W25Q_SPI_DMA_TX_STREAM_IRQn        DMA2_Stream3_IRQn
#define W25Q_SPI_DMA_RX_STREAM_IRQHandler  DMA2_Stream2_IRQHandler
#define W25Q_SPI_DMA_TX_STREAM_IRQHandler  DMA2_Stream3_IRQHandler

#define W25Q_SPI_SCK_PIN                   GPIO_Pin_5
#define W25Q_SPI_SCK_GPIO_PORT             GPIOA
#define W25Q_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define W25Q_SPI_SCK_SOURCE                GPIO_PinSource5
#define W25Q_SPI_SCK_AF                    GPIO_AF_SPI1

#define W25Q_SPI_MISO_PIN                  GPIO_Pin_6
#define W25Q_SPI_MISO_GPIO_PORT            GPIOA
#define W25Q_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define W25Q_SPI_MISO_SOURCE               GPIO_PinSource6
#define W25Q_SPI_MISO_AF                   GPIO_AF_SPI1

#define W25Q_SPI_MOSI_PIN                  GPIO_Pin_7
#define W25Q_SPI_MOSI_GPIO_PORT            GPIOA
#define W25Q_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define W25Q_SPI_MOSI_SOURCE               GPIO_PinSource7
#define W25Q_SPI_MOSI_AF                   GPIO_AF_SPI1

#define W25Q_SPI_CS_PIN                    GPIO_Pin_4
#define W25Q_SPI_CS_GPIO_PORT              GPIOA
#define W25Q_SPI_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

/* Select W25Q FLASH: Chip Select pin low */
#define W25Q_SPI_CS_ENABLE()       GPIO_ResetBits(W25Q_SPI_CS_GPIO_PORT, W25Q_SPI_CS_PIN)
/* Deselect W25Q FLASH: Chip Select pin high */
#define W25Q_SPI_CS_DISABLE()      GPIO_SetBits(W25Q_SPI_CS_GPIO_PORT, W25Q_SPI_CS_PIN)

#define W25Q_SPI_SLOW_RATE         SPI_BaudRatePrescaler_128 /* 84 / 128 = 0.65625MHz */
#define W25Q_SPI_FAST_RATE         SPI_BaudRatePrescaler_8   /* 84 / 8 = 10.5MHz */

void w25q_spi_init(void);
void w25q_spi_config_rate(uint32_t scalingfactor);
void spi_rx_tx_dma(uint8_t *w, uint8_t *r, uint16_t l);
void spi_rx_tx_dma_util(uint8_t *w, uint8_t *r, uint16_t l);

void w25q_int_isr(void);
void w25q_INT_Callback(void);

#endif /* __W25Q_SPI_H */
