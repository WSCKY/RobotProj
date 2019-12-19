/*
 * magif.c
 *
 *  Created on: Dec 18, 2019
 *      Author: kychu
 */

#include "magif.h"

/* I2C handler declaration */
static I2C_HandleTypeDef MAG_I2cHandle;

static DMA_HandleTypeDef hdma_tx;
static DMA_HandleTypeDef hdma_rx;

static uint32_t magif_initialized = 0;

#if FREERTOS_ENABLED
static osMutexId if_mutex = NULL;
#else
#endif /* FREERTOS_ENABLED */

/* I2C TIMING Register define when I2C clock source is PCLK1 */
/* I2C TIMING is calculated in case of the I2C Clock source is the PCLK1 = 54 MHz */
/* This example use TIMING to 0x00601B5E to reach 400 KHz speed (Rise time = 20ns, Fall time = 20ns) */
#define I2C_TIMING      0x00601B5E /* 400KHz */
//#define I2C_TIMING      0x1070699D /* 100KHz */

status_t magif_init(void)
{
  if(magif_initialized != 0) return status_ok;
  /*##-1- Configure the I2C peripheral ######################################*/
  MAG_I2cHandle.Instance              = MAG_I2C;
  MAG_I2cHandle.Init.Timing           = I2C_TIMING;
  MAG_I2cHandle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
  MAG_I2cHandle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
  MAG_I2cHandle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
  MAG_I2cHandle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
  MAG_I2cHandle.Init.OwnAddress1      = 0xFF;
  MAG_I2cHandle.Init.OwnAddress2      = 0xFF;

  if(HAL_I2C_Init(&MAG_I2cHandle) != HAL_OK) return status_error; /* Initialization Error */

  /* Enable the Analog I2C Filter */
  if(HAL_I2CEx_ConfigAnalogFilter(&MAG_I2cHandle,I2C_ANALOGFILTER_ENABLE) != HAL_OK) return status_error;

#if FREERTOS_ENABLED
  /* Create the mutex  */
  osMutexDef(MAGIFMutex);
  if_mutex = osMutexCreate(osMutex(MAGIFMutex));
  if(if_mutex == NULL) return status_error;
#else
#endif /* FREERTOS_ENABLED */

  magif_initialized = 1;
  return status_ok;
}

status_t magif_check_ready(void)
{
  if(HAL_I2C_GetState(&MAG_I2cHandle) != HAL_I2C_STATE_READY)
    return status_busy;
  return status_ok;
}

status_t magif_tx_bytes_dma(uint16_t DevAddress, uint8_t *pData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  return (status_t)HAL_I2C_Master_Transmit_DMA(&MAG_I2cHandle, DevAddress, pData, Size);
}

status_t magif_read_mem_dma(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  return (status_t)HAL_I2C_Mem_Read_DMA(&MAG_I2cHandle, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size);
}

status_t magif_write_mem_dma(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreTakeFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
  return (status_t)HAL_I2C_Mem_Write_DMA(&MAG_I2cHandle, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size);
}

void magif_txcplt_callback(I2C_HandleTypeDef *hi2c)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void magif_rxcplt_callback(I2C_HandleTypeDef *hi2c)
{
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
}

void magif_error_callback(I2C_HandleTypeDef *hi2c)
{
//  if (HAL_I2C_GetError(I2cHandle) != HAL_I2C_ERROR_AF) {
#if FREERTOS_ENABLED
  portBASE_TYPE taskWoken = pdFALSE;
  xSemaphoreGiveFromISR(if_mutex, &taskWoken);
#else
#endif /* FREERTOS_ENABLED */
//  }
}

void magif_msp_init(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  RCC_PeriphCLKInitTypeDef  RCC_PeriphCLKInitStruct;

  /*##-1- Configure the I2C clock source. The clock is derived from the SYSCLK #*/
  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2Cx;
  RCC_PeriphCLKInitStruct.I2c1ClockSelection = RCC_I2CxCLKSOURCE_SYSCLK;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

  /*##-2- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  I2Cx_SCL_GPIO_CLK_ENABLE();
  I2Cx_SDA_GPIO_CLK_ENABLE();
  /* Enable I2Cx clock */
  I2Cx_CLK_ENABLE();

  /* Enable DMAx clock */
  I2Cx_DMA_CLK_ENABLE();

  /*##-3- Configure peripheral GPIO ##########################################*/
  /* I2C TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SCL_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
  HAL_GPIO_Init(I2Cx_SCL_GPIO_PORT, &GPIO_InitStruct);

  /* I2C RX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = I2Cx_SDA_PIN;
  GPIO_InitStruct.Alternate = I2Cx_SCL_SDA_AF;
  HAL_GPIO_Init(I2Cx_SDA_GPIO_PORT, &GPIO_InitStruct);

  /*##-4- Configure the DMA Channels #########################################*/
  /* Configure the DMA handler for Transmission process */
  hdma_tx.Instance                 = I2Cx_DMA_INSTANCE_TX;
  hdma_tx.Init.Channel             = I2Cx_DMA_CHANNEL_TX;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_MEDIUM;
  hdma_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */
  hdma_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_tx.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  hdma_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */

  HAL_DMA_Init(&hdma_tx);

  /* Associate the initialized DMA handle to the the I2C handle */
  __HAL_LINKDMA(hi2c, hdmatx, hdma_tx);

  /* Configure the DMA handler for Transmission process */
  hdma_rx.Instance                 = I2Cx_DMA_INSTANCE_RX;
  hdma_rx.Init.Channel             = I2Cx_DMA_CHANNEL_RX;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_NORMAL;
  hdma_rx.Init.Priority            = DMA_PRIORITY_MEDIUM;
  hdma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;           /* FIFO mode disabled               */
  hdma_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
  hdma_rx.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
  hdma_rx.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst                 */

  HAL_DMA_Init(&hdma_rx);

  /* Associate the initialized DMA handle to the the I2C handle */
  __HAL_LINKDMA(hi2c, hdmarx, hdma_rx);

  /*##-5- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (I2Cx_TX) */
  HAL_NVIC_SetPriority(I2Cx_DMA_TX_IRQn, INT_PRIO_MAGIF_DMATX,0);
  HAL_NVIC_EnableIRQ(I2Cx_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (I2Cx_RX) */
  HAL_NVIC_SetPriority(I2Cx_DMA_RX_IRQn, INT_PRIO_MAGIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(I2Cx_DMA_RX_IRQn);

  /*##-6- Configure the NVIC for I2C ########################################*/
  /* NVIC for I2Cx */
  HAL_NVIC_SetPriority(I2Cx_ER_IRQn, INT_PRIO_MAGIF_ER, 0);
  HAL_NVIC_EnableIRQ(I2Cx_ER_IRQn);
  HAL_NVIC_SetPriority(I2Cx_EV_IRQn, INT_PRIO_MAGIF_EV, 0);
  HAL_NVIC_EnableIRQ(I2Cx_EV_IRQn);
}

void magif_msp_deinit(I2C_HandleTypeDef *hi2c)
{
  /*##-1- Reset peripherals ##################################################*/
  I2Cx_FORCE_RESET();
  I2Cx_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure I2C Tx as alternate function  */
  HAL_GPIO_DeInit(I2Cx_SCL_GPIO_PORT, I2Cx_SCL_PIN);
  /* Configure I2C Rx as alternate function  */
  HAL_GPIO_DeInit(I2Cx_SDA_GPIO_PORT, I2Cx_SDA_PIN);

  /*##-3- Disable the DMA Channels ###########################################*/
  /* De-Initialize the DMA Channel associated to transmission process */
  HAL_DMA_DeInit(&hdma_tx);
  /* De-Initialize the DMA Channel associated to reception process */
  HAL_DMA_DeInit(&hdma_rx);

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(I2Cx_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(I2Cx_DMA_RX_IRQn);

  /*##-5- Disable the NVIC for I2C ##########################################*/
  HAL_NVIC_DisableIRQ(I2Cx_ER_IRQn);
  HAL_NVIC_DisableIRQ(I2Cx_EV_IRQn);
}

/**
  * @brief  This function handles I2C event interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is related to I2C data transmission
  */
void I2Cx_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&MAG_I2cHandle);
}

/**
  * @brief  This function handles I2C error interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is related to I2C error
  */
void I2Cx_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&MAG_I2cHandle);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is related to DMA Channel
  *         used for I2C data transmission
  */
void I2Cx_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(MAG_I2cHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  * @Note   This function is related to DMA Channel
  *         used for I2C data reception
  */
void I2Cx_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(MAG_I2cHandle.hdmatx);
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
