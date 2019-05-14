#ifndef __IMU_SPI_H
#define __IMU_SPI_H

#include "SysConfig.h"

#define IMU_SPI_DEV_ID_A            0
#define IMU_SPI_DEV_ID_G            1
#define IMU_SPI_DEV_ID_M            2

/* IMU Sensor SPI Interface pins  */
#define IMU_SPI                           SPI2
#define IMU_SPI_CLK                       RCC_APB1Periph_SPI2
#define IMU_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd

#define IMU_SPI_DMA_CLK                   RCC_AHB1Periph_DMA1
#define IMU_SPI_DMA_CHANNEL               DMA_Channel_0
#define IMU_SPI_DMA_RX_STREAM             DMA1_Stream3
#define IMU_SPI_DMA_TX_STREAM             DMA1_Stream4
#define IMU_SPI_DMA_RX_TCIF               DMA_IT_TCIF3
#define IMU_SPI_DMA_TX_TCIF               DMA_IT_TCIF4
//#define IMU_SPI_DMA_RX_STREAM_IRQn        DMA1_Stream3_IRQn
//#define IMU_SPI_DMA_TX_STREAM_IRQn        DMA1_Stream4_IRQn

#define IMU_SPI_SCK_PIN                   GPIO_Pin_13
#define IMU_SPI_SCK_GPIO_PORT             GPIOB
#define IMU_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define IMU_SPI_SCK_SOURCE                GPIO_PinSource13
#define IMU_SPI_SCK_AF                    GPIO_AF_SPI2

#define IMU_SPI_MISO_PIN                  GPIO_Pin_14
#define IMU_SPI_MISO_GPIO_PORT            GPIOB
#define IMU_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define IMU_SPI_MISO_SOURCE               GPIO_PinSource14
#define IMU_SPI_MISO_AF                   GPIO_AF_SPI2

#define IMU_SPI_MOSI_PIN                  GPIO_Pin_15
#define IMU_SPI_MOSI_GPIO_PORT            GPIOB
#define IMU_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define IMU_SPI_MOSI_SOURCE               GPIO_PinSource15
#define IMU_SPI_MOSI_AF                   GPIO_AF_SPI2

#define IMU_SPI_CS_A_PIN                  GPIO_Pin_2
#define IMU_SPI_CS_A_GPIO_PORT            GPIOB
#define IMU_SPI_CS_A_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define IMU_SPI_CS_G_PIN                  GPIO_Pin_2
#define IMU_SPI_CS_G_GPIO_PORT            GPIOB
#define IMU_SPI_CS_G_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define IMU_SPI_CS_M_PIN                  GPIO_Pin_10
#define IMU_SPI_CS_M_GPIO_PORT            GPIOB
#define IMU_SPI_CS_M_GPIO_CLK             RCC_AHB1Periph_GPIOB

#define IMU_INT_1_PIN                     GPIO_Pin_8
#define IMU_INT_1_GPIO_PORT               GPIOA
#define IMU_INT_1_GPIO_CLK                RCC_AHB1Periph_GPIOA
#define IMU_INT_1_EXTI_PORT_SOURCE        EXTI_PortSourceGPIOA
#define IMU_INT_1_EXTI_PIN_SOURCE         EXTI_PinSource8
#define IMU_INT_1_EXTI_LINE               EXTI_Line8
#define IMU_INT_1_EXTI_IRQn               EXTI9_5_IRQn

#define IMU_INT_2_PIN                     GPIO_Pin_9
#define IMU_INT_2_GPIO_PORT               GPIOA
#define IMU_INT_2_GPIO_CLK                RCC_AHB1Periph_GPIOA
#define IMU_INT_2_EXTI_PORT_SOURCE        EXTI_PortSourceGPIOA
#define IMU_INT_2_EXTI_PIN_SOURCE         EXTI_PinSource9
#define IMU_INT_2_EXTI_LINE               EXTI_Line9
#define IMU_INT_2_EXTI_IRQn               EXTI9_5_IRQn

#define IMU_INT_M_PIN                     GPIO_Pin_12
#define IMU_INT_M_GPIO_PORT               GPIOB
#define IMU_INT_M_GPIO_CLK                RCC_AHB1Periph_GPIOB
#define IMU_INT_M_EXTI_PORT_SOURCE        EXTI_PortSourceGPIOB
#define IMU_INT_M_EXTI_PIN_SOURCE         EXTI_PinSource12
#define IMU_INT_M_EXTI_LINE               EXTI_Line12
#define IMU_INT_M_EXTI_IRQn               EXTI15_10_IRQn

#define IMU_DRDY_M_PIN                    GPIO_Pin_8
#define IMU_DRDY_M_GPIO_PORT              GPIOB
#define IMU_DRDY_M_GPIO_CLK               RCC_AHB1Periph_GPIOB
#define IMU_DRDY_M_EXTI_PORT_SOURCE       EXTI_PortSourceGPIOB
#define IMU_DRDY_M_EXTI_PIN_SOURCE        EXTI_PinSource8
#define IMU_DRDY_M_EXTI_LINE              EXTI_Line8
#define IMU_DRDY_M_EXTI_IRQn              EXTI9_5_IRQn

/* Select IMU: Chip Select pin low */
#define IMU_SPI_CS_A_ENABLE()       GPIO_ResetBits(IMU_SPI_CS_A_GPIO_PORT, IMU_SPI_CS_A_PIN)
#define IMU_SPI_CS_G_ENABLE()       GPIO_ResetBits(IMU_SPI_CS_G_GPIO_PORT, IMU_SPI_CS_G_PIN)
#define IMU_SPI_CS_M_ENABLE()       GPIO_ResetBits(IMU_SPI_CS_M_GPIO_PORT, IMU_SPI_CS_M_PIN)

#define IMU_SPI_CS_ENABLE(id)       do {      if((id) == IMU_SPI_DEV_ID_A) IMU_SPI_CS_A_ENABLE(); \
	                                     else if((id) == IMU_SPI_DEV_ID_G) IMU_SPI_CS_G_ENABLE(); \
	                                     else if((id) == IMU_SPI_DEV_ID_M) IMU_SPI_CS_M_ENABLE(); \
	                                     else {} } while(0)
/* Deselect IMU: Chip Select pin high */
#define IMU_SPI_CS_A_DISABLE()      GPIO_SetBits(IMU_SPI_CS_A_GPIO_PORT, IMU_SPI_CS_A_PIN)
#define IMU_SPI_CS_G_DISABLE()      GPIO_SetBits(IMU_SPI_CS_G_GPIO_PORT, IMU_SPI_CS_G_PIN)
#define IMU_SPI_CS_M_DISABLE()      GPIO_SetBits(IMU_SPI_CS_M_GPIO_PORT, IMU_SPI_CS_M_PIN)

#define IMU_SPI_CS_DISABLE(id)      do {      if((id) == IMU_SPI_DEV_ID_A) IMU_SPI_CS_A_DISABLE(); \
	                                     else if((id) == IMU_SPI_DEV_ID_G) IMU_SPI_CS_G_DISABLE(); \
	                                     else if((id) == IMU_SPI_DEV_ID_M) IMU_SPI_CS_M_DISABLE(); \
	                                     else {} } while(0)

#define IMU_SPI_SLOW_RATE           SPI_BaudRatePrescaler_64 /* 84 / 64 = 1.3125MHz */
#define IMU_SPI_FAST_RATE           SPI_BaudRatePrescaler_16   /* 84 / 16 = 5.25MHz */

void imu_spi_init(void);
void imu_spi_config_rate(uint32_t scalingfactor);
void spi_rx_tx_dma(uint8_t *w, uint8_t *r, uint16_t l, uint8_t id);
void spi_rx_tx_dma_util(uint8_t *w, uint8_t *r, uint16_t l, uint8_t id);

void imu_int_1_isr(void);
void imu_int_2_isr(void);
void imu_int_m_isr(void);
void imu_drdy_m_isr(void);
void imu_spi_dma_rx_isr(void);
void imu_spi_dma_tx_isr(void);
void imu_int_1_callback(void) __attribute__((weak));
void imu_int_2_callback(void) __attribute__((weak));
void imu_int_m_callback(void) __attribute__((weak));
void imu_drdy_m_callback(void) __attribute__((weak));

#endif /* __IMU_SPI_H */
