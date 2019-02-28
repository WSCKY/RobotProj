/*
 * spi1.h
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#ifndef DRIVER_PERIPH_SPI1_H_
#define DRIVER_PERIPH_SPI1_H_

#include "SysConfig.h"

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

#define SPI1_NSS_ENABLE()                (SPI1_NSS_GPIO_PORT->BRR = SPI1_NSS_PIN)
#define SPI1_NSS_DISABLE()               (SPI1_NSS_GPIO_PORT->BSRR = SPI1_NSS_PIN)

#define SPI1_DATASIZE                    SPI_DataSize_8b
#define SPI1_DATAMASK                    (uint8_t)0xFF

void spi1_init(void);
void spi1_configrate(uint32_t scalingfactor);
void spi1_rx_tx(uint8_t *w, uint8_t *r, uint16_t l);

#endif /* DRIVER_PERIPH_SPI1_H_ */
