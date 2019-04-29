/*
 * w25q_spi.c
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#include "w25q_spi.h"

static uint8_t _tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

#if FREERTOS_ENABLED
static SemaphoreHandle_t xSemaphore = NULL;
#endif /* FREERTOS_ENABLED */

static void w25q_spi_mutex_init(void);

/**
  * @brief  Initializes the peripherals used by the IMU Sensor driver.
  * @param  None
  * @retval None
  */
void w25q_spi_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  w25q_spi_mutex_init();
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#endif /* FREERTOS_ENABLED */

  /*!< Enable the SPI clock */
  W25Q_SPI_CLK_INIT(W25Q_SPI_CLK, ENABLE);
  /*!< Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(W25Q_SPI_DMA_CLK, ENABLE);
  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(W25Q_SPI_SCK_GPIO_CLK | W25Q_SPI_MISO_GPIO_CLK | W25Q_SPI_MOSI_GPIO_CLK | W25Q_SPI_CS_GPIO_CLK, ENABLE);

  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */
  GPIO_PinAFConfig(W25Q_SPI_SCK_GPIO_PORT, W25Q_SPI_SCK_SOURCE, W25Q_SPI_SCK_AF);
  GPIO_PinAFConfig(W25Q_SPI_MISO_GPIO_PORT, W25Q_SPI_MISO_SOURCE, W25Q_SPI_MISO_AF);
  GPIO_PinAFConfig(W25Q_SPI_MOSI_GPIO_PORT, W25Q_SPI_MOSI_SOURCE, W25Q_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = W25Q_SPI_SCK_PIN;
  GPIO_Init(W25Q_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  W25Q_SPI_MOSI_PIN;
  GPIO_Init(W25Q_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  W25Q_SPI_MISO_PIN;
  GPIO_Init(W25Q_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure W25Q FLASH CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = W25Q_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(W25Q_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /*!< Deselect the IMU: Chip Select high */
  W25Q_SPI_CS_DISABLE();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = W25Q_SPI_SLOW_RATE;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(W25Q_SPI, &SPI_InitStructure);

  /*!< Enable W25Q_SPI DMA Rx&Tx request */
  SPI_I2S_DMACmd(W25Q_SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);

  /*!< Enable the W25Q_SPI  */
  SPI_Cmd(W25Q_SPI, ENABLE);

  /*<! Configure W25Q_SPI DMA RX&TX STREAM */
  DMA_DeInit(W25Q_SPI_DMA_RX_STREAM);
  DMA_Cmd(W25Q_SPI_DMA_RX_STREAM, DISABLE);
  DMA_DeInit(W25Q_SPI_DMA_TX_STREAM);
  DMA_Cmd(W25Q_SPI_DMA_TX_STREAM, DISABLE);

  DMA_InitStructure.DMA_Channel = W25Q_SPI_DMA_CHANNEL;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(W25Q_SPI->DR));
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc     = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode      = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority  = DMA_Priority_VeryHigh;
  DMA_InitStructure.DMA_FIFOMode      = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(W25Q_SPI_DMA_RX_STREAM, &DMA_InitStructure);

  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  DMA_Init(W25Q_SPI_DMA_TX_STREAM, &DMA_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = W25Q_SPI_DMA_RX_STREAM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_W25Q_SPI_DMA_RX;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = W25Q_SPI_DMA_TX_STREAM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_W25Q_SPI_DMA_TX;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA RX&TX Transfer complete interrupt */
  DMA_ITConfig(W25Q_SPI_DMA_RX_STREAM, DMA_IT_TC, ENABLE);
  DMA_ITConfig(W25Q_SPI_DMA_TX_STREAM, DMA_IT_TC, ENABLE);

#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#endif /* FREERTOS_ENABLED */
}

/*
 * configure spi bit rate.
 * params: W25Q_SPI_SLOW_RATE, W25Q_SPI_FAST_RATE
 */
void W25Q_spi_config_rate(uint32_t scalingfactor)
{
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#endif /* FREERTOS_ENABLED */
	uint16_t tmpreg = 0;
	if(scalingfactor != W25Q_SPI_FAST_RATE && scalingfactor != W25Q_SPI_SLOW_RATE) return;
	/* Disable the selected SPI peripheral */
	W25Q_SPI->CR1 &= 0xFFBF;
	/* Get the SPIx CR1 value */
	tmpreg = W25Q_SPI->CR1;
	/* Clear BR[2:0] bits */
	tmpreg &= 0xFFC7;
	/* Set the scaling bits */
	tmpreg |= scalingfactor;
	/* Write to SPIx CR1 */
	W25Q_SPI->CR1 = tmpreg;
	/* Enable the selected SPI peripheral */
	W25Q_SPI->CR1 |= 0x0040;
#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#endif /* FREERTOS_ENABLED */
}

/*
 * read & write any bytes through spi with dma.
 */
#pragma GCC push_options
#pragma GCC optimize ("O0")
void spi_rx_tx_dma(uint8_t *w, uint8_t *r, uint16_t l)
{
#if FREERTOS_ENABLED
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreTakeFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#endif /* FREERTOS_ENABLED */
  while (_tx_comp_flag == 0);
  _tx_comp_flag = 0;
  W25Q_SPI_CS_ENABLE();
  DMA_InitStructure.DMA_BufferSize      = l;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)r;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_PeripheralToMemory;
  DMA_Init(W25Q_SPI_DMA_RX_STREAM, &DMA_InitStructure);
  DMA_Cmd(W25Q_SPI_DMA_RX_STREAM, ENABLE);

  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)w;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
  DMA_Init(W25Q_SPI_DMA_TX_STREAM, &DMA_InitStructure);
  DMA_Cmd(W25Q_SPI_DMA_TX_STREAM, ENABLE);
#if FREERTOS_ENABLED
  xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#endif /* FREERTOS_ENABLED */
}

/*
 * read & write any bytes through spi with dma until complete.
 */
void spi_rx_tx_dma_util(uint8_t *w, uint8_t *r, uint16_t l)
{
#if FREERTOS_ENABLED
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreTakeFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#endif /* FREERTOS_ENABLED */
  while (_tx_comp_flag == 0);
  _tx_comp_flag = 0;
  W25Q_SPI_CS_ENABLE();
  DMA_InitStructure.DMA_BufferSize      = l;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)r;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_PeripheralToMemory;
  DMA_Init(W25Q_SPI_DMA_RX_STREAM, &DMA_InitStructure);
  DMA_Cmd(W25Q_SPI_DMA_RX_STREAM, ENABLE);

  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)w;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
  DMA_Init(W25Q_SPI_DMA_TX_STREAM, &DMA_InitStructure);
  DMA_Cmd(W25Q_SPI_DMA_TX_STREAM, ENABLE);
  while (_tx_comp_flag == 0);
#if FREERTOS_ENABLED
  xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#endif /* FREERTOS_ENABLED */
}
#pragma GCC pop_options

static void w25q_spi_mutex_init(void)
{
#if FREERTOS_ENABLED
  xSemaphore = xSemaphoreCreateMutex();
#endif /* FREERTOS_ENABLED */
}

void W25Q_SPI_DMA_RX_STREAM_IRQHandler(void)
{
	if(DMA_GetFlagStatus(W25Q_SPI_DMA_RX_STREAM, W25Q_SPI_DMA_RX_TCIF)) {
		DMA_Cmd(W25Q_SPI_DMA_RX_STREAM, DISABLE);
		_tx_comp_flag = 1;
		W25Q_SPI_CS_DISABLE();
		DMA_ClearFlag(W25Q_SPI_DMA_RX_STREAM, W25Q_SPI_DMA_RX_TCIF);
	}
}

void W25Q_SPI_DMA_TX_STREAM_IRQHandler(void)
{
	if (DMA_GetFlagStatus(W25Q_SPI_DMA_TX_STREAM, W25Q_SPI_DMA_TX_TCIF)) {
		DMA_Cmd(W25Q_SPI_DMA_TX_STREAM, DISABLE);
		DMA_ClearFlag(W25Q_SPI_DMA_TX_STREAM, W25Q_SPI_DMA_TX_TCIF);
	}
}
