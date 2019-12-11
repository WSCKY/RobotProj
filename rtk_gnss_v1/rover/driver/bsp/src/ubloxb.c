#include "ubloxb.h"
#include "string.h"

/* UART handler declaration */
static UART_HandleTypeDef UBXB_UartHandle;
static DMA_HandleTypeDef UBXB_TxDmaHandle;
static DMA_HandleTypeDef UBXB_RxDmaHandle;

static __IO bool_t _ubxb_tx_ready = true;

//static uint8_t ubxbif_cache[UBXBIF_CACHE_DEPTH];
static uint8_t *ubxbif_cache = NULL;
static uint32_t _ubxb_data_available = 0, _ubxb_ptr_out = 0;

status_t ubxbif_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /* -2- Configure IO in output push-pull mode to drive external Pins */
  GPIO_InitStruct.Pin   = GPIO_PIN_9;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // RST pin
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // DO NOT RESET.
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  UBXB_UartHandle.Instance        = UBXB_UART;

  UBXB_UartHandle.Init.BaudRate   = 38400;
  UBXB_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UBXB_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UBXB_UartHandle.Init.Parity     = UART_PARITY_NONE;
  UBXB_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UBXB_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&UBXB_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&UBXB_UartHandle) != HAL_OK) return status_error;

//  memset(ubxbif_cache, 0x00, UBXBIF_CACHE_DEPTH);
  ubxbif_cache = kmm_alloc(UBXBIF_CACHE_DEPTH);
  if(ubxbif_cache == NULL) return status_nomem;

  __HAL_UART_ENABLE_IT(&UBXB_UartHandle, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&UBXB_UartHandle, (uint8_t *)ubxbif_cache, UBXBIF_CACHE_DEPTH);
  return status_ok;
}

void ubxbif_reset_enable(void)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
}

void ubxbif_reset_disable(void)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
}

status_t ubxbif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&UBXB_UartHandle, (uint8_t*)p, l);
}

status_t ubxbif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_ubxb_tx_ready == false) {}
  _ubxb_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&UBXB_UartHandle, (uint8_t*)p, l);
  while(_ubxb_tx_ready == false) {}
  return ret;
}

uint32_t ubxbif_rx_bytes(uint8_t *p, uint32_t l)
{
  uint32_t len = l;
  if(_ubxb_data_available < l) len = _ubxb_data_available;
  for(uint32_t cnt = 0; cnt < len; cnt ++) {
    p[cnt] = ubxbif_cache[_ubxb_ptr_out ++];
    _ubxb_data_available --;
    if(_ubxb_ptr_out == UBXBIF_CACHE_DEPTH) _ubxb_ptr_out = 0;
  }
  return len;
}

status_t ubxbif_set_baudrate(uint32_t baudrate)
{
  status_t ret = status_ok;
  while(_ubxb_tx_ready == false) {}
  ret = (status_t)HAL_UART_AbortReceive(&UBXB_UartHandle); if(ret != status_ok) return ret;
  __HAL_UART_DISABLE_IT(&UBXB_UartHandle, UART_IT_IDLE);
  UBXB_UartHandle.Init.BaudRate = baudrate;
  ret = (status_t)HAL_UART_Init(&UBXB_UartHandle); if(ret != status_ok) return ret;
  _ubxb_ptr_out = 0;
  _ubxb_data_available = 0;
  __HAL_UART_ENABLE_IT(&UBXB_UartHandle, UART_IT_IDLE);
  return (status_t)HAL_UART_Receive_DMA(&UBXB_UartHandle, (uint8_t *)ubxbif_cache, UBXBIF_CACHE_DEPTH);
}

void ubxbif_txcplt_callback(UART_HandleTypeDef *huart)
{
  _ubxb_tx_ready = true;
}

void ubxbif_rxcplt_callback(UART_HandleTypeDef *huart)
{
}

static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

    uint32_t current_pos = UBXBIF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(current_pos >= _ubxb_ptr_out)
      _ubxb_data_available = current_pos - _ubxb_ptr_out;
    else
      _ubxb_data_available = current_pos + UBXBIF_CACHE_DEPTH - _ubxb_ptr_out;
  }
}

void ubxbif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  UBXB_UART_TX_GPIO_CLK_ENABLE();
  UBXB_UART_RX_GPIO_CLK_ENABLE();

  /* Enable UBXB_UART clock */
  UBXB_UART_CLK_ENABLE();

  /* Enable DMA clock */
  UBXB_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = UBXB_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = UBXB_UART_TX_AF;

  HAL_GPIO_Init(UBXB_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = UBXB_UART_RX_PIN;
  GPIO_InitStruct.Alternate = UBXB_UART_RX_AF;

  HAL_GPIO_Init(UBXB_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  UBXB_TxDmaHandle.Instance                 = UBXB_UART_TX_DMA_STREAM;
  UBXB_TxDmaHandle.Init.Channel             = UBXB_UART_TX_DMA_CHANNEL;
  UBXB_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  UBXB_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  UBXB_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  UBXB_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  UBXB_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  UBXB_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  UBXB_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&UBXB_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, UBXB_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  UBXB_RxDmaHandle.Instance                 = UBXB_UART_RX_DMA_STREAM;
  UBXB_RxDmaHandle.Init.Channel             = UBXB_UART_RX_DMA_CHANNEL;
  UBXB_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  UBXB_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  UBXB_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  UBXB_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  UBXB_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  UBXB_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  UBXB_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&UBXB_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, UBXB_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART6_TX) */
  HAL_NVIC_SetPriority(UBXB_UART_DMA_TX_IRQn, INT_PRIO_UBXBIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(UBXB_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (USART6_RX) */
  HAL_NVIC_SetPriority(UBXB_UART_DMA_RX_IRQn, INT_PRIO_UBXBIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(UBXB_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(UBXB_UART_IRQn, INT_PRIO_UBXBIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(UBXB_UART_IRQn);
}

void ubxbif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  UBXB_UART_FORCE_RESET();
  UBXB_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UBXB_UART Tx as alternate function  */
  HAL_GPIO_DeInit(UBXB_UART_TX_GPIO_PORT, UBXB_UART_TX_PIN);
  /* Configure UBXB_UART Rx as alternate function  */
  HAL_GPIO_DeInit(UBXB_UART_RX_GPIO_PORT, UBXB_UART_RX_PIN);

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
  HAL_NVIC_DisableIRQ(UBXB_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(UBXB_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UBXB_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UBXB_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void UBXB_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(UBXB_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void UBXB_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&UBXB_UartHandle);
  HAL_UART_IRQHandler(&UBXB_UartHandle);
}
