#ifndef __IMU_SPI_H
#define __IMU_SPI_H

#include "SysConfig.h"

/* IMU Sensor SPI Interface pins  */
#define IMU_SPI                           SPI1
#define IMU_SPI_CLK                       RCC_APB2Periph_SPI1
#define IMU_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define IMU_SPI_DMA_CLK                   RCC_AHB1Periph_DMA2
#define IMU_SPI_DMA_CHANNEL               DMA_Channel_3
#define IMU_SPI_DMA_RX_STREAM             DMA2_Stream2
#define IMU_SPI_DMA_TX_STREAM             DMA2_Stream3
#define IMU_SPI_DMA_RX_TCIF               DMA_IT_TCIF2
#define IMU_SPI_DMA_TX_TCIF               DMA_IT_TCIF3
#define IMU_SPI_DMA_RX_STREAM_IRQn        DMA2_Stream2_IRQn
#define IMU_SPI_DMA_TX_STREAM_IRQn        DMA2_Stream3_IRQn
#define IMU_SPI_DMA_RX_STREAM_IRQHandler  DMA2_Stream2_IRQHandler
#define IMU_SPI_DMA_TX_STREAM_IRQHandler  DMA2_Stream3_IRQHandler

#define IMU_SPI_SCK_PIN                   GPIO_Pin_5
#define IMU_SPI_SCK_GPIO_PORT             GPIOA
#define IMU_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define IMU_SPI_SCK_SOURCE                GPIO_PinSource5
#define IMU_SPI_SCK_AF                    GPIO_AF_SPI1

#define IMU_SPI_MISO_PIN                  GPIO_Pin_6
#define IMU_SPI_MISO_GPIO_PORT            GPIOA
#define IMU_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define IMU_SPI_MISO_SOURCE               GPIO_PinSource6
#define IMU_SPI_MISO_AF                   GPIO_AF_SPI1

#define IMU_SPI_MOSI_PIN                  GPIO_Pin_7
#define IMU_SPI_MOSI_GPIO_PORT            GPIOA
#define IMU_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define IMU_SPI_MOSI_SOURCE               GPIO_PinSource7
#define IMU_SPI_MOSI_AF                   GPIO_AF_SPI1

#define IMU_SPI_CS_PIN                    GPIO_Pin_4
#define IMU_SPI_CS_GPIO_PORT              GPIOA
#define IMU_SPI_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define IMU_INT_PIN                       GPIO_Pin_0
#define IMU_INT_GPIO_PORT                 GPIOB
#define IMU_INT_GPIO_CLK                  RCC_AHB1Periph_GPIOB
#define IMU_INT_EXTI_PORT_SOURCE          EXTI_PortSourceGPIOB
#define IMU_INT_EXTI_PIN_SOURCE           EXTI_PinSource0
#define IMU_INT_EXTI_LINE                 EXTI_Line0
#define IMU_INT_EXTI_IRQn                 EXTI0_IRQn

/* Select IMU: Chip Select pin low */
#define IMU_SPI_CS_ENABLE()       GPIO_ResetBits(IMU_SPI_CS_GPIO_PORT, IMU_SPI_CS_PIN)
/* Deselect IMU: Chip Select pin high */
#define IMU_SPI_CS_DISABLE()      GPIO_SetBits(IMU_SPI_CS_GPIO_PORT, IMU_SPI_CS_PIN)

#define IMU_SPI_SLOW_RATE         SPI_BaudRatePrescaler_128 /* 84 / 128 = 0.65625MHz */
#define IMU_SPI_FAST_RATE         SPI_BaudRatePrescaler_8   /* 84 / 8 = 10.5MHz */

void imu_spi_init(void);
void imu_spi_config_rate(uint32_t scalingfactor);
void spi_rx_tx_dma(uint8_t *w, uint8_t *r, uint16_t l);
void spi_rx_tx_dma_util(uint8_t *w, uint8_t *r, uint16_t l);

void imu_int_isr(void);
void IMU_INT_Callback(void);

#endif /* __IMU_SPI_H */
