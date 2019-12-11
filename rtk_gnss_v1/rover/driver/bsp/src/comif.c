#include "comif.h"
#include <string.h>

/* UART handler declaration */
static UART_HandleTypeDef UartHandle;
static DMA_HandleTypeDef TxDmaHandle;
static DMA_HandleTypeDef RxDmaHandle;

static __IO bool_t comif_tx_ready = true;

//static uint8_t comif_cache[COMIF_CACHE_DEPTH];
static uint8_t *comif_cache = NULL;
static uint32_t _data_available = 0, _ptr_out = 0;

status_t comif_init(void)
{
//  HAL_StatusTypeDef ret = HAL_OK;
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UartHandle.Instance        = COM_UART;

  UartHandle.Init.BaudRate   = 115200;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&UartHandle) != HAL_OK) return status_error;

//  memset(comif_cache, 0x00, COMIF_CACHE_DEPTH);
  comif_cache = kmm_alloc(COMIF_CACHE_DEPTH);
  if(comif_cache == NULL) return status_nomem;
//  SCB_CleanDCache();
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)comif_cache, COMIF_CACHE_DEPTH);
  return status_ok;
}

status_t comif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)p, l);
}

status_t comif_tx_string_util(const char *p)
{
  return comif_tx_bytes_util((uint8_t *)p, strlen((const char *)p));
}

status_t comif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while (comif_tx_ready == false) {}
  comif_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&UartHandle, (uint8_t*)p, l);
  while (comif_tx_ready == false) {}
  return ret;
}

uint32_t comif_rx_bytes(uint8_t *p, uint32_t l)
{
  uint32_t len = l;
  if(_data_available < l) len = _data_available;
  for(uint32_t cnt = 0; cnt < len; cnt ++) {
    p[cnt] = comif_cache[_ptr_out ++];
    _data_available --;
    if(_ptr_out == COMIF_CACHE_DEPTH) _ptr_out = 0;
  }
  return len;
}

status_t comif_set_baudrate(uint32_t baudrate)
{
  status_t ret = status_ok;
  ret = (status_t)HAL_UART_AbortReceive(&UartHandle); if(ret != status_ok) return ret;
  __HAL_UART_DISABLE_IT(&UartHandle, UART_IT_IDLE);
  UartHandle.Init.BaudRate = baudrate;
  ret = (status_t)HAL_UART_Init(&UartHandle); if(ret != status_ok) return ret;
  _ptr_out = 0;
  _data_available = 0;
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_IDLE);
  return (status_t)HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)comif_cache, COMIF_CACHE_DEPTH);
}

void comif_txcplt_callback(UART_HandleTypeDef *huart)
{
  comif_tx_ready = true;
}

void comif_rxcplt_callback(UART_HandleTypeDef *huart)
{

}

static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

    uint32_t pos = COMIF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(pos >= _ptr_out) _data_available = pos - _ptr_out;
    else _data_available = pos + COMIF_CACHE_DEPTH - _ptr_out;
  }
}

void comif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  COM_UART_TX_GPIO_CLK_ENABLE();
  COM_UART_RX_GPIO_CLK_ENABLE();

  /* Enable COM_UART clock */
  COM_UART_CLK_ENABLE();

  /* Enable DMA clock */
  COM_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = COM_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = COM_UART_TX_AF;

  HAL_GPIO_Init(COM_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = COM_UART_RX_PIN;
  GPIO_InitStruct.Alternate = COM_UART_RX_AF;

  HAL_GPIO_Init(COM_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  TxDmaHandle.Instance                 = COM_UART_TX_DMA_STREAM;
  TxDmaHandle.Init.Channel             = COM_UART_TX_DMA_CHANNEL;
  TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  TxDmaHandle.Init.Mode                = DMA_NORMAL;
  TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, TxDmaHandle);

  /* Configure the DMA handler for reception process */
  RxDmaHandle.Instance                 = COM_UART_RX_DMA_STREAM;
  RxDmaHandle.Init.Channel             = COM_UART_RX_DMA_CHANNEL;
  RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART2_TX) */
  HAL_NVIC_SetPriority(COM_UART_DMA_TX_IRQn, INT_PRIO_COMIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(COM_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (USART2_RX) */
  HAL_NVIC_SetPriority(COM_UART_DMA_RX_IRQn, INT_PRIO_COMIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(COM_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(COM_UART_IRQn, INT_PRIO_COMIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(COM_UART_IRQn);
}

void comif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  COM_UART_FORCE_RESET();
  COM_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure COM_UART Tx as alternate function  */
  HAL_GPIO_DeInit(COM_UART_TX_GPIO_PORT, COM_UART_TX_PIN);
  /* Configure COM_UART Rx as alternate function  */
  HAL_GPIO_DeInit(COM_UART_RX_GPIO_PORT, COM_UART_RX_PIN);

  /*##-3- Disable the DMA #####################################################*/
  /* De-Initialize the DMA channel associated to reception process */
  if(huart->hdmarx != 0) {
    HAL_DMA_DeInit(huart->hdmarx);
  }
  /* De-Initialize the DMA channel associated to transmission process */
  if(huart->hdmatx != 0) {
    HAL_DMA_DeInit(huart->hdmatx);
  }

  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(COM_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(COM_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void COM_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void COM_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void COM_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&UartHandle);
  HAL_UART_IRQHandler(&UartHandle);
}
