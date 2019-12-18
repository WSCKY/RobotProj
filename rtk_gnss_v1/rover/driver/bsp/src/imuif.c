/*
 * imuif.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "imuif.h"

/* SPI handler declaration */
static SPI_HandleTypeDef IMU_SpiHandle;

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
#else
#endif /* FREERTOS_ENABLED */

#define IMU_SPI_CS_SELECT()    HAL_GPIO_WritePin(IMU_SPI_NSS_GPIO_PORT, IMU_SPI_NSS_PIN, GPIO_PIN_RESET)
#define IMU_SPI_CS_DESELECT()  HAL_GPIO_WritePin(IMU_SPI_NSS_GPIO_PORT, IMU_SPI_NSS_PIN, GPIO_PIN_SET)

status_t imuif_init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  IMU_SpiHandle.Instance               = IMU_SPI;
  IMU_SpiHandle.Init.Mode              = SPI_MODE_MASTER;
  IMU_SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  IMU_SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  IMU_SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE; // MODE 0
  IMU_SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  IMU_SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  IMU_SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  IMU_SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  IMU_SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  IMU_SpiHandle.Init.CRCPolynomial     = 7;
  IMU_SpiHandle.Init.NSS               = SPI_NSS_SOFT;

  if(HAL_SPI_Init(&IMU_SpiHandle) != HAL_OK) return status_error; /* Initialization Error */

#if FREERTOS_ENABLED
  /* Create the mutex  */
  osMutexDef(IMUIFMutex);
  if_mutex = osMutexCreate(osMutex(IMUIFMutex));
  if(if_mutex == NULL) return status_error;
#else
#endif /* FREERTOS_ENABLED */

  return status_ok;
}

status_t imuif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  status_t ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  IMU_SPI_CS_SELECT();
  ret = (status_t)HAL_SPI_TransmitReceive(&IMU_SpiHandle, pTxData, pRxData, Size, Size);
  IMU_SPI_CS_DESELECT();
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t imuif_txrx_bytes_dma(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  IMU_SPI_CS_SELECT();
  return (status_t)HAL_SPI_TransmitReceive_DMA(&IMU_SpiHandle, pTxData, pRxData, Size);
}

void imuif_rxtxcplt_callback(SPI_HandleTypeDef *hspi)
{
  IMU_SPI_CS_DESELECT();
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void imuif_msp_init(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  IMU_SPI_SCK_GPIO_CLK_ENABLE();
  IMU_SPI_NSS_GPIO_CLK_ENABLE();
  IMU_SPI_MISO_GPIO_CLK_ENABLE();
  IMU_SPI_MOSI_GPIO_CLK_ENABLE();
  IMU_INT1_GPIO_CLK_ENABLE();
  IMU_INT2_GPIO_CLK_ENABLE();
  /* Enable SPI1 clock */
  IMU_SPI_CLK_ENABLE();
  /* Enable DMA clock */
  IMU_SPI_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI NSS GPIO pin configuration */
  GPIO_InitStruct.Pin       = IMU_SPI_NSS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(IMU_SPI_NSS_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(IMU_SPI_NSS_GPIO_PORT, IMU_SPI_NSS_PIN, GPIO_PIN_SET);

  /* IMU INT1 GPIO pin configuration */
  GPIO_InitStruct.Pin       = IMU_INT1_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT1_GPIO_PORT, &GPIO_InitStruct);

  /* IMU INT2 GPIO pin configuration */
  GPIO_InitStruct.Pin       = IMU_INT2_PIN;
  HAL_GPIO_Init(IMU_INT2_GPIO_PORT, &GPIO_InitStruct);

  /* SPI SCK GPIO pin configuration */
  GPIO_InitStruct.Pin       = IMU_SPI_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = IMU_SPI_SCK_AF;
  HAL_GPIO_Init(IMU_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MISO GPIO pin configuration */
  GPIO_InitStruct.Pin = IMU_SPI_MISO_PIN;
  GPIO_InitStruct.Alternate = IMU_SPI_MISO_AF;
  HAL_GPIO_Init(IMU_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration */
  GPIO_InitStruct.Pin = IMU_SPI_MOSI_PIN;
  GPIO_InitStruct.Alternate = IMU_SPI_MOSI_AF;
  HAL_GPIO_Init(IMU_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = IMU_SPI_TX_DMA_STREAM;
  hdma_tx.Init.Channel             = IMU_SPI_TX_DMA_CHANNEL;
  hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_tx.Init.PeriphBurst         = DMA_PBURST_INC4;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(hspi, hdmatx, hdma_tx);

  /* Configure the DMA handler for Transmission process */
  hdma_rx.Instance                 = IMU_SPI_RX_DMA_STREAM;

  hdma_rx.Init.Channel             = IMU_SPI_RX_DMA_CHANNEL;
  hdma_rx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  hdma_rx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst            = DMA_MBURST_INC4;
  hdma_rx.Init.PeriphBurst         = DMA_PBURST_INC4;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&hdma_rx);

  /* Associate the initialized DMA handle to the the SPI handle */
  __HAL_LINKDMA(hspi, hdmarx, hdma_rx);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (IMU_SPI_TX) */
  HAL_NVIC_SetPriority(IMU_SPI_DMA_TX_IRQn, INT_PRIO_IMUIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(IMU_SPI_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (IMU_SPI_RX) */
  HAL_NVIC_SetPriority(IMU_SPI_DMA_RX_IRQn, INT_PRIO_IMUIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(IMU_SPI_DMA_RX_IRQn);

  /* Enable and set EXTI line Interrupt priority for IMU INT1 */
  HAL_NVIC_SetPriority(IMU_INT1_GPIO_EXTI_IRQn, INT_PRIO_IMUIF_INT1, 0);
  HAL_NVIC_EnableIRQ(IMU_INT1_GPIO_EXTI_IRQn);

  /* Enable and set EXTI line Interrupt priority for IMU INT2 */
  HAL_NVIC_SetPriority(IMU_INT2_GPIO_EXTI_IRQn, INT_PRIO_IMUIF_INT2, 0);
  HAL_NVIC_EnableIRQ(IMU_INT2_GPIO_EXTI_IRQn);
}

void imuif_msp_deinit(SPI_HandleTypeDef *hspi)
{
  /*##-1- Reset peripherals ##################################################*/
  IMU_SPI_FORCE_RESET();
  IMU_SPI_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Deconfigure SPI NSS */
  HAL_GPIO_DeInit(IMU_SPI_NSS_GPIO_PORT, IMU_SPI_NSS_PIN);
  /* Deconfigure SPI SCK */
  HAL_GPIO_DeInit(IMU_SPI_SCK_GPIO_PORT, IMU_SPI_SCK_PIN);
  /* Deconfigure SPI MISO */
  HAL_GPIO_DeInit(IMU_SPI_MISO_GPIO_PORT, IMU_SPI_MISO_PIN);
  /* Deconfigure SPI MOSI */
  HAL_GPIO_DeInit(IMU_SPI_MOSI_GPIO_PORT, IMU_SPI_MOSI_PIN);

  /*##-3- Disable the DMA ####################################################*/
  /* De-Initialize the DMA associated to transmission process */
  HAL_DMA_DeInit(&hdma_tx);
  /* De-Initialize the DMA associated to reception process */
  HAL_DMA_DeInit(&hdma_rx);

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(IMU_SPI_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(IMU_SPI_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA TX interrupt request.
  * @param  None
  * @retval None
  */
void IMU_SPI_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(IMU_SpiHandle.hdmatx);
}

/**
  * @brief  This function handles DMA RX interrupt request.
  * @param  None
  * @retval None
  */
void IMU_SPI_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(IMU_SpiHandle.hdmarx);
}

__weak void imuif_int1_callback(void) {}
__weak void imuif_int2_callback(void) {}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
