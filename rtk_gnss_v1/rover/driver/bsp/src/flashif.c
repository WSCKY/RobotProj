/*
 * imuif.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "flashif.h"

/* SPI handler declaration */
static SPI_HandleTypeDef FLASH_SpiHandle;

#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
#else
#endif /* FREERTOS_ENABLED */

status_t flashif_init(void)
{
  /*##-1- Configure the SPI peripheral #######################################*/
  /* Set the SPI parameters */
  FLASH_SpiHandle.Instance               = FLASH_SPI;
  FLASH_SpiHandle.Init.Mode              = SPI_MODE_MASTER;
  FLASH_SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  FLASH_SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  FLASH_SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE; // MODE 0
  FLASH_SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  FLASH_SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  FLASH_SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  FLASH_SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  FLASH_SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  FLASH_SpiHandle.Init.CRCPolynomial     = 7;
  FLASH_SpiHandle.Init.NSS               = SPI_NSS_SOFT;

  if(HAL_SPI_Init(&FLASH_SpiHandle) != HAL_OK) return status_error; /* Initialization Error */

#if FREERTOS_ENABLED
  /* Create the mutex  */
  osMutexDef(IMUIFMutex);
  if_mutex = osMutexCreate(osMutex(IMUIFMutex));
  if(if_mutex == NULL) return status_error;
#else
#endif /* FREERTOS_ENABLED */

  return status_ok;
}

status_t flashif_select(uint8_t id)
{
  (void)id;
  HAL_GPIO_WritePin(FLASH_SPI_NSS_GPIO_PORT, FLASH_SPI_NSS_PIN, GPIO_PIN_RESET);
  return status_ok;
}

status_t flashif_deselect(uint8_t id)
{
  (void)id;
  HAL_GPIO_WritePin(FLASH_SPI_NSS_GPIO_PORT, FLASH_SPI_NSS_PIN, GPIO_PIN_SET);
  return status_ok;
}

status_t flashif_tx_bytes(uint8_t *pTxData, uint16_t Size)
{
  status_t ret = status_ok;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  ret = (status_t)HAL_SPI_Transmit(&FLASH_SpiHandle, pTxData, Size, Size);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t flashif_txrx_bytes(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
  status_t ret;
#if FREERTOS_ENABLED
  osMutexWait(if_mutex, osWaitForever);
#else
#endif /* FREERTOS_ENABLED */
  ret = (status_t)HAL_SPI_TransmitReceive(&FLASH_SpiHandle, pTxData, pRxData, Size, Size);
#if FREERTOS_ENABLED
  osMutexRelease(if_mutex);
#else
#endif /* FREERTOS_ENABLED */
  return ret;
}

status_t flashif_txrx_bytes_it(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  flashif_select(0);
  return (status_t)HAL_SPI_TransmitReceive_IT(&FLASH_SpiHandle, pTxData, pRxData, Size);
}

void flashif_rxtxcplt_callback(SPI_HandleTypeDef *hspi)
{
  flashif_deselect(0);
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void flashif_msp_init(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  FLASH_SPI_SCK_GPIO_CLK_ENABLE();
  FLASH_SPI_NSS_GPIO_CLK_ENABLE();
  FLASH_SPI_MISO_GPIO_CLK_ENABLE();
  FLASH_SPI_MOSI_GPIO_CLK_ENABLE();
  FLASH_WP_GPIO_CLK_ENABLE();
  /* Enable FLASH_SPI clock */
  FLASH_SPI_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/
  /* SPI NSS GPIO pin configuration */
  GPIO_InitStruct.Pin       = FLASH_SPI_NSS_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(FLASH_SPI_NSS_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(FLASH_SPI_NSS_GPIO_PORT, FLASH_SPI_NSS_PIN, GPIO_PIN_SET);

  /* FLASH WP GPIO pin configuration */
  GPIO_InitStruct.Pin       = FLASH_WP_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(FLASH_WP_GPIO_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(FLASH_WP_GPIO_PORT, FLASH_WP_PIN, GPIO_PIN_SET);

  /* SPI SCK GPIO pin configuration */
  GPIO_InitStruct.Pin       = FLASH_SPI_SCK_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = FLASH_SPI_SCK_AF;
  HAL_GPIO_Init(FLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MISO GPIO pin configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_MISO_PIN;
  GPIO_InitStruct.Alternate = FLASH_SPI_MISO_AF;
  HAL_GPIO_Init(FLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStruct);

  /* SPI MOSI GPIO pin configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_MOSI_PIN;
  GPIO_InitStruct.Alternate = FLASH_SPI_MOSI_AF;
  HAL_GPIO_Init(FLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the NVIC for SPI #########################################*/
  /* NVIC for SPI */
  HAL_NVIC_SetPriority(FLASH_SPI_IRQn, INT_PRIO_FLASHIF_IT, 0);
  NVIC_EnableIRQ(FLASH_SPI_IRQn);
}

void flashif_msp_deinit(SPI_HandleTypeDef *hspi)
{
  /*##-1- Reset peripherals ##################################################*/
  FLASH_SPI_FORCE_RESET();
  FLASH_SPI_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* Deconfigure SPI NSS */
  HAL_GPIO_DeInit(FLASH_SPI_NSS_GPIO_PORT, FLASH_SPI_NSS_PIN);
  /* Deconfigure SPI SCK */
  HAL_GPIO_DeInit(FLASH_SPI_SCK_GPIO_PORT, FLASH_SPI_SCK_PIN);
  /* Deconfigure SPI MISO */
  HAL_GPIO_DeInit(FLASH_SPI_MISO_GPIO_PORT, FLASH_SPI_MISO_PIN);
  /* Deconfigure SPI MOSI */
  HAL_GPIO_DeInit(FLASH_SPI_MOSI_GPIO_PORT, FLASH_SPI_MOSI_PIN);
  /* Deconfigure FLASH WP */
  HAL_GPIO_DeInit(FLASH_WP_GPIO_PORT, FLASH_WP_PIN);
}

/**
  * @brief  This function handles SPI interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_SPI_IRQHandler(void)
{
  HAL_SPI_IRQHandler(&FLASH_SpiHandle);
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
