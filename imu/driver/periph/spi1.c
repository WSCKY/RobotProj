/*
 * spi1.c
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#include "spi1.h"

static uint8_t MCU_SPI_WriteRead(uint8_t Data);

/**
  * @brief  Configure SPI1 peripheral.
	* @param  None
	* @retval None
	*/
void spi1_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  /* Enable SCK, MOSI, MISO and NSS GPIO clocks */
  RCC_AHBPeriphClockCmd(SPI1_SCK_GPIO_CLK | SPI1_MISO_GPIO_CLK | SPI1_MOSI_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(SPI1_SCK_GPIO_PORT, SPI1_SCK_SOURCE, SPI1_SCK_AF);
  GPIO_PinAFConfig(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_SOURCE, SPI1_MOSI_AF);
  GPIO_PinAFConfig(SPI1_MISO_GPIO_PORT, SPI1_MISO_SOURCE, SPI1_MISO_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN;
  GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  SPI1_MOSI_PIN;
  GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = SPI1_MISO_PIN;
  GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI NSS pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = SPI1_NSS_PIN;
  GPIO_Init(SPI1_NSS_GPIO_PORT, &GPIO_InitStructure);

  SPI1_NSS_DISABLE();

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(SPI1);
  /* Initializes the SPI communication */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI1_DATASIZE;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; /* 48 / 64 = 750KHz */
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  /* Initialize the FIFO threshold */
  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

  /* Enable the SPI peripheral */
  SPI_Cmd(SPI1, ENABLE);
}

/*
 * read & write any bytes through spi with dma.
 */
void spi1_rx_tx(uint8_t *w, uint8_t *r, uint16_t l)
{
	uint32_t i = 0;
	SPI1_NSS_ENABLE();
	for(i = 0; i < l; i ++) {
		r[i] = MCU_SPI_WriteRead(w[i]);
	}
	SPI1_NSS_DISABLE();
}

void spi1_configrate(uint32_t scalingfactor)
{
	uint16_t tmpreg = 0;
	/* Disable the selected SPI peripheral */
	SPI1->CR1 &= 0xFFBF;
	/* Get the SPIx CR1 value */
	tmpreg = SPI1->CR1;
	/* Clear BR[2:0] bits */
	tmpreg &= 0xFFC7;
	/* Set the scaling bits */
	tmpreg |= scalingfactor;
	/* Write to SPIx CR1 */
	SPI1->CR1 = tmpreg;
	/* Enable the selected SPI peripheral */
	SPI1->CR1 |= 0x0040;
}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  Data: byte send.
  * @retval The received byte value
  * @retval None
  */
static uint8_t MCU_SPI_WriteRead(uint8_t Data)
{
  uint8_t tmp = 0x00;

  /* Wait until the transmit buffer is empty */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) != SET) {}

  /* Send the byte */
  SPI_SendData8(SPI1, Data);

  /* Wait to receive a byte */
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) != SET) {}

  /* Return the byte read from the SPI bus */
  tmp = SPI_ReceiveData8(SPI1);

  /* Return read Data */
  return tmp;
}
