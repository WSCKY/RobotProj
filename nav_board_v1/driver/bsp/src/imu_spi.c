#include "imu_spi.h"

static uint8_t _tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

#if FREERTOS_ENABLED
static SemaphoreHandle_t xSemaphore = NULL;
#endif /* FREERTOS_ENABLED */

static void imu_spi_mutex_init(void);

/**
  * @brief  Initializes the peripherals used by the IMU Sensor driver.
  * @param  None
  * @retval None
  */
void imu_spi_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  imu_spi_mutex_init();
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#endif /* FREERTOS_ENABLED */

  /*!< Enable the SPI clock */
  IMU_SPI_CLK_INIT(IMU_SPI_CLK, ENABLE);
	/*!< Enable the DMA clock */
	RCC_AHB1PeriphClockCmd(IMU_SPI_DMA_CLK, ENABLE);
  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(IMU_SPI_SCK_GPIO_CLK | IMU_SPI_MISO_GPIO_CLK | 
    IMU_SPI_MOSI_GPIO_CLK | IMU_SPI_CS_GPIO_CLK | IMU_INT_GPIO_CLK, ENABLE);

  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(IMU_SPI_SCK_GPIO_PORT, IMU_SPI_SCK_SOURCE, IMU_SPI_SCK_AF);
  GPIO_PinAFConfig(IMU_SPI_MISO_GPIO_PORT, IMU_SPI_MISO_SOURCE, IMU_SPI_MISO_AF);
  GPIO_PinAFConfig(IMU_SPI_MOSI_GPIO_PORT, IMU_SPI_MOSI_SOURCE, IMU_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;

	/*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = IMU_SPI_SCK_PIN;
  GPIO_Init(IMU_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  IMU_SPI_MOSI_PIN;
  GPIO_Init(IMU_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  IMU_SPI_MISO_PIN;
  GPIO_Init(IMU_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure IMU Sensor CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = IMU_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IMU_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

	/*!< Configure IMU Sensor INT pin in output pushpull mode ********************/
	GPIO_InitStructure.GPIO_Pin = IMU_INT_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(IMU_INT_GPIO_PORT, &GPIO_InitStructure);
  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(IMU_INT_EXTI_PORT_SOURCE, IMU_INT_EXTI_PIN_SOURCE);

  /*!< Deselect the IMU: Chip Select high */
  IMU_SPI_CS_DISABLE();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = IMU_SPI_SLOW_RATE;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(IMU_SPI, &SPI_InitStructure);

	/*!< Enable IMU_SPI DMA Rx&Tx request */
	SPI_I2S_DMACmd(IMU_SPI, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);

  /*!< Enable the IMU_SPI  */
  SPI_Cmd(IMU_SPI, ENABLE);

	/*<! Configure IMU_SPI DMA RX&TX STREAM */
	DMA_DeInit(IMU_SPI_DMA_RX_STREAM);
	DMA_Cmd(IMU_SPI_DMA_RX_STREAM, DISABLE);
	DMA_DeInit(IMU_SPI_DMA_TX_STREAM);
	DMA_Cmd(IMU_SPI_DMA_TX_STREAM, DISABLE);

	DMA_InitStructure.DMA_Channel = IMU_SPI_DMA_CHANNEL;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(IMU_SPI->DR));
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
	DMA_Init(IMU_SPI_DMA_RX_STREAM, &DMA_InitStructure);

	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_Init(IMU_SPI_DMA_TX_STREAM, &DMA_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = IMU_SPI_DMA_RX_STREAM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_IMU_SPI_DMA_RX;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = IMU_SPI_DMA_TX_STREAM_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_IMU_SPI_DMA_TX;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable DMA RX&TX Transfer complete interrupt */
	DMA_ITConfig(IMU_SPI_DMA_RX_STREAM, DMA_IT_TC, ENABLE);
	DMA_ITConfig(IMU_SPI_DMA_TX_STREAM, DMA_IT_TC, ENABLE);

  /* Configure IMU INT EXTI line */
  EXTI_InitStructure.EXTI_Line = IMU_INT_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = IMU_INT_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_IMU_INT_PIN;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#endif /* FREERTOS_ENABLED */
}

/*
 * configure spi bit rate.
 * params: IMU_SPI_SLOW_RATE, IMU_SPI_FAST_RATE
 */
void imu_spi_config_rate(uint32_t scalingfactor)
{
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#endif /* FREERTOS_ENABLED */
	uint16_t tmpreg = 0;
	if(scalingfactor != IMU_SPI_FAST_RATE && scalingfactor != IMU_SPI_SLOW_RATE) return;
	/* Disable the selected SPI peripheral */
	IMU_SPI->CR1 &= 0xFFBF;
	/* Get the SPIx CR1 value */
	tmpreg = IMU_SPI->CR1;
	/* Clear BR[2:0] bits */
	tmpreg &= 0xFFC7;
	/* Set the scaling bits */
	tmpreg |= scalingfactor;
	/* Write to SPIx CR1 */
	IMU_SPI->CR1 = tmpreg;
	/* Enable the selected SPI peripheral */
	IMU_SPI->CR1 |= 0x0040;
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
  IMU_SPI_CS_ENABLE();
  DMA_InitStructure.DMA_BufferSize      = l;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)r;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_PeripheralToMemory;
  DMA_Init(IMU_SPI_DMA_RX_STREAM, &DMA_InitStructure);
  DMA_Cmd(IMU_SPI_DMA_RX_STREAM, ENABLE);

  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)w;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
  DMA_Init(IMU_SPI_DMA_TX_STREAM, &DMA_InitStructure);
  DMA_Cmd(IMU_SPI_DMA_TX_STREAM, ENABLE);
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
  IMU_SPI_CS_ENABLE();
  DMA_InitStructure.DMA_BufferSize      = l;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)r;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_PeripheralToMemory;
  DMA_Init(IMU_SPI_DMA_RX_STREAM, &DMA_InitStructure);
  DMA_Cmd(IMU_SPI_DMA_RX_STREAM, ENABLE);

  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)w;
  DMA_InitStructure.DMA_DIR             = DMA_DIR_MemoryToPeripheral;
  DMA_Init(IMU_SPI_DMA_TX_STREAM, &DMA_InitStructure);
  DMA_Cmd(IMU_SPI_DMA_TX_STREAM, ENABLE);
  while (_tx_comp_flag == 0);
#if FREERTOS_ENABLED
  xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#endif /* FREERTOS_ENABLED */
}
#pragma GCC pop_options

void imu_int_isr(void)
{
  if(EXTI_GetITStatus(IMU_INT_EXTI_LINE) != RESET) {
    IMU_INT_Callback();
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(IMU_INT_EXTI_LINE);
  }
}

static void imu_spi_mutex_init(void)
{
#if FREERTOS_ENABLED
  xSemaphore = xSemaphoreCreateMutex();
#endif /* FREERTOS_ENABLED */
}

void IMU_SPI_DMA_RX_STREAM_IRQHandler(void)
{
	if(DMA_GetFlagStatus(IMU_SPI_DMA_RX_STREAM, IMU_SPI_DMA_RX_TCIF)) {
		DMA_Cmd(IMU_SPI_DMA_RX_STREAM, DISABLE);
		_tx_comp_flag = 1;
		IMU_SPI_CS_DISABLE();
		DMA_ClearFlag(IMU_SPI_DMA_RX_STREAM, IMU_SPI_DMA_RX_TCIF);
	}
}

void IMU_SPI_DMA_TX_STREAM_IRQHandler(void)
{
	if (DMA_GetFlagStatus(IMU_SPI_DMA_TX_STREAM, IMU_SPI_DMA_TX_TCIF)) {
		DMA_Cmd(IMU_SPI_DMA_TX_STREAM, DISABLE);
		DMA_ClearFlag(IMU_SPI_DMA_TX_STREAM, IMU_SPI_DMA_TX_TCIF);
	}
}
