#include "ubloxa.h"
#include "string.h"

/* UART handler declaration */
static UART_HandleTypeDef UBXA_UartHandle;
static DMA_HandleTypeDef UBXA_TxDmaHandle;
static DMA_HandleTypeDef UBXA_RxDmaHandle;

static __IO bool_t _ubxa_tx_ready = true;

//static uint8_t ubxaif_cache[UBXAIF_CACHE_DEPTH];
static uint8_t *ubxaif_cache = NULL;
static uint32_t _ubxa_data_available = 0, _ubxa_ptr_out = 0;

status_t ubxaif_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* -2- Configure IO in output push-pull mode to drive external Pins */
  GPIO_InitStruct.Pin   = GPIO_PIN_8;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); // RST pin
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET); // DO NOT RESET.
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UBXA_UartHandle.Instance        = UBXA_UART;

  UBXA_UartHandle.Init.BaudRate   = 38400;
  UBXA_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UBXA_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UBXA_UartHandle.Init.Parity     = UART_PARITY_NONE;
  UBXA_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UBXA_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&UBXA_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&UBXA_UartHandle) != HAL_OK) return status_error;

//  memset(ubxaif_cache, 0x00, UBXAIF_CACHE_DEPTH);
  ubxaif_cache = kmm_alloc(UBXAIF_CACHE_DEPTH);
  if(ubxaif_cache == NULL) return status_nomem;

  __HAL_UART_ENABLE_IT(&UBXA_UartHandle, UART_IT_IDLE);
  if(HAL_UART_Receive_DMA(&UBXA_UartHandle, (uint8_t *)ubxaif_cache, UBXAIF_CACHE_DEPTH) != HAL_OK) return status_error;
  return status_ok;
}

void ubxaif_reset_enable(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}

void ubxaif_reset_disable(void)
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}

status_t ubxaif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&UBXA_UartHandle, (uint8_t*)p, l);
}

status_t ubxaif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_ubxa_tx_ready == false) {}
  _ubxa_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&UBXA_UartHandle, (uint8_t*)p, l);
  while(_ubxa_tx_ready == false) {}
  return ret;
}

uint32_t ubxaif_rx_bytes(uint8_t *p, uint32_t l)
{
  uint32_t len = l;
  if(_ubxa_data_available < l) len = _ubxa_data_available;
  for(uint32_t cnt = 0; cnt < len; cnt ++) {
    p[cnt] = ubxaif_cache[_ubxa_ptr_out ++];
    _ubxa_data_available --;
    if(_ubxa_ptr_out == UBXAIF_CACHE_DEPTH) _ubxa_ptr_out = 0;
  }
  return len;
}

status_t ubxaif_set_baudrate(uint32_t baudrate)
{
  status_t ret = status_ok;
  while(_ubxa_tx_ready == false) {}
  ret = (status_t)HAL_UART_AbortReceive(&UBXA_UartHandle); if(ret != status_ok) return ret;
  __HAL_UART_DISABLE_IT(&UBXA_UartHandle, UART_IT_IDLE);
  UBXA_UartHandle.Init.BaudRate = baudrate;
  ret = (status_t)HAL_UART_Init(&UBXA_UartHandle); if(ret != status_ok) return ret;
  _ubxa_ptr_out = 0;
  _ubxa_data_available = 0;
  __HAL_UART_ENABLE_IT(&UBXA_UartHandle, UART_IT_IDLE);
  return (status_t)HAL_UART_Receive_DMA(&UBXA_UartHandle, (uint8_t *)ubxaif_cache, UBXAIF_CACHE_DEPTH);
}

void ubxaif_txcplt_callback(UART_HandleTypeDef *huart)
{
  _ubxa_tx_ready = true;
}

void ubxaif_rxcplt_callback(UART_HandleTypeDef *huart)
{
}

static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

    uint32_t current_pos = UBXAIF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(current_pos >= _ubxa_ptr_out)
      _ubxa_data_available = current_pos - _ubxa_ptr_out;
    else
      _ubxa_data_available = current_pos + UBXAIF_CACHE_DEPTH - _ubxa_ptr_out;
  }
}

void ubxaif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  UBXA_UART_TX_GPIO_CLK_ENABLE();
  UBXA_UART_RX_GPIO_CLK_ENABLE();

  /* Enable UBXA_UART clock */
  UBXA_UART_CLK_ENABLE();

  /* Enable DMA clock */
  UBXA_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = UBXA_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = UBXA_UART_TX_AF;

  HAL_GPIO_Init(UBXA_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = UBXA_UART_RX_PIN;
  GPIO_InitStruct.Alternate = UBXA_UART_RX_AF;

  HAL_GPIO_Init(UBXA_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  UBXA_TxDmaHandle.Instance                 = UBXA_UART_TX_DMA_STREAM;
  UBXA_TxDmaHandle.Init.Channel             = UBXA_UART_TX_DMA_CHANNEL;
  UBXA_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  UBXA_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  UBXA_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  UBXA_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  UBXA_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  UBXA_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  UBXA_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&UBXA_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, UBXA_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  UBXA_RxDmaHandle.Instance                 = UBXA_UART_RX_DMA_STREAM;
  UBXA_RxDmaHandle.Init.Channel             = UBXA_UART_RX_DMA_CHANNEL;
  UBXA_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  UBXA_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  UBXA_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  UBXA_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  UBXA_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  UBXA_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  UBXA_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&UBXA_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, UBXA_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART1_TX) */
  HAL_NVIC_SetPriority(UBXA_UART_DMA_TX_IRQn, INT_PRIO_UBXAIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(UBXA_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
  HAL_NVIC_SetPriority(UBXA_UART_DMA_RX_IRQn, INT_PRIO_UBXAIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(UBXA_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(UBXA_UART_IRQn, INT_PRIO_UBXAIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(UBXA_UART_IRQn);
}

void ubxaif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  UBXA_UART_FORCE_RESET();
  UBXA_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UBXA_UART Tx as alternate function  */
  HAL_GPIO_DeInit(UBXA_UART_TX_GPIO_PORT, UBXA_UART_TX_PIN);
  /* Configure UBXA_UART Rx as alternate function  */
  HAL_GPIO_DeInit(UBXA_UART_RX_GPIO_PORT, UBXA_UART_RX_PIN);

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
  HAL_NVIC_DisableIRQ(UBXA_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(UBXA_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UBXA_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UBXA_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UBXA_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UBXA_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void UBXA_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&UBXA_UartHandle);
  HAL_UART_IRQHandler(&UBXA_UartHandle);
}
