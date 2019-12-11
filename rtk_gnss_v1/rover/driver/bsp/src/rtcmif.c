#include "rtcmif.h"
#include "string.h"

#include "ringbuffer.h"

/* UART handler declaration */
static UART_HandleTypeDef RTCMIF_UartHandle;
static DMA_HandleTypeDef RTCMIF_TxDmaHandle;
static DMA_HandleTypeDef RTCMIF_RxDmaHandle;

static __IO bool_t _rtcmif_tx_ready = true;

#define RTCM_RB_CAPACITY                           1024
static ringbuffer_handle rtcm_rb;
//static uint8_t ringbuffer_cache[RTCM_RB_CAPACITY];
//static uint8_t rtcmif_cache[RTCM_CACHE_DEPTH];
static uint8_t *ringbuffer_cache = NULL;
static uint8_t *rtcmif_cache = NULL;
//static uint32_t _rtcmif_data_available = 0, _rtcmif_ptr_out = 0;

status_t rtcmif_init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  RTCMIF_UartHandle.Instance        = RTCM_UART;

  RTCMIF_UartHandle.Init.BaudRate   = 38400;
  RTCMIF_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  RTCMIF_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  RTCMIF_UartHandle.Init.Parity     = UART_PARITY_NONE;
  RTCMIF_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  RTCMIF_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&RTCMIF_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&RTCMIF_UartHandle) != HAL_OK) return status_error;

//  memset(rtcmif_cache, 0x00, RTCM_CACHE_DEPTH);
  rtcmif_cache = kmm_alloc(RTCM_CACHE_DEPTH);
  if(rtcmif_cache == NULL) return status_nomem;
  ringbuffer_cache = kmm_alloc(RTCM_RB_CAPACITY);
  if(ringbuffer_cache == NULL) return status_nomem;

  ringbuffer_init(&rtcm_rb, ringbuffer_cache, RTCM_RB_CAPACITY);
  __HAL_UART_ENABLE_IT(&RTCMIF_UartHandle, UART_IT_IDLE);
  if(HAL_UART_Receive_DMA(&RTCMIF_UartHandle, (uint8_t *)rtcmif_cache, RTCM_CACHE_DEPTH) != HAL_OK) return status_error;
  return status_ok;
}

status_t rtcmif_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&RTCMIF_UartHandle, (uint8_t*)p, l);
}

status_t rtcmif_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_rtcmif_tx_ready == false) {}
  _rtcmif_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&RTCMIF_UartHandle, (uint8_t*)p, l);
  while(_rtcmif_tx_ready == false) {}
  return ret;
}

uint32_t rtcmif_rx_bytes(uint8_t *p, uint32_t l)
{
//  uint32_t len = l;
//  if(_rtcmif_data_available < l) len = _rtcmif_data_available;
//  for(uint32_t cnt = 0; cnt < len; cnt ++) {
//    p[cnt] = rtcmif_cache[_rtcmif_ptr_out ++];
//    _rtcmif_data_available --;
//    if(_rtcmif_ptr_out == RTCM_CACHE_DEPTH) _rtcmif_ptr_out = 0;
//  }
//  return len;
  return ringbuffer_poll(&rtcm_rb, p, l);
}

uint32_t rtcmif_cache_usage(void)
{
  return ringbuffer_usage(&rtcm_rb);
}

void rtcmif_txcplt_callback(UART_HandleTypeDef *huart)
{
  _rtcmif_tx_ready = true;
}

void rtcmif_rxcplt_callback(UART_HandleTypeDef *huart)
{
//  ringbuffer_push(&rtcm_rb, rtcmif_cache, RTCM_CACHE_DEPTH);
}
//uint32_t max_remain = 0;
//uint32_t idle_receive = 0;
static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

//    uint32_t current_pos = RTCM_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
//    if(current_pos >= _rtcmif_ptr_out)
//      _rtcmif_data_available = current_pos - _rtcmif_ptr_out;
//    else
//      _rtcmif_data_available = current_pos + RTCM_CACHE_DEPTH - _rtcmif_ptr_out;
    uint32_t len = RTCM_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
//    if(len > max_remain) max_remain = len;
//    idle_receive = len;
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(&rtcm_rb, rtcmif_cache, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, RTCM_CACHE_DEPTH);
      __HAL_DMA_ENABLE(huart->hdmarx);
//      __HAL_DMA_CLEAR_FLAG(huart->hdmarx, DMA_FLAG_TCIF0_4);
    }
  }
}

void rtcmif_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  RTCM_UART_TX_GPIO_CLK_ENABLE();
  RTCM_UART_RX_GPIO_CLK_ENABLE();

  /* Enable RTCM_UART clock */
  RTCM_UART_CLK_ENABLE();

  /* Enable DMA clock */
  RTCM_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = RTCM_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = RTCM_UART_TX_AF;

  HAL_GPIO_Init(RTCM_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = RTCM_UART_RX_PIN;
  GPIO_InitStruct.Alternate = RTCM_UART_RX_AF;

  HAL_GPIO_Init(RTCM_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  RTCMIF_TxDmaHandle.Instance                 = RTCM_UART_TX_DMA_STREAM;
  RTCMIF_TxDmaHandle.Init.Channel             = RTCM_UART_TX_DMA_CHANNEL;
  RTCMIF_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  RTCMIF_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  RTCMIF_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  RTCMIF_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  RTCMIF_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  RTCMIF_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  RTCMIF_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&RTCMIF_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, RTCMIF_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  RTCMIF_RxDmaHandle.Instance                 = RTCM_UART_RX_DMA_STREAM;
  RTCMIF_RxDmaHandle.Init.Channel             = RTCM_UART_RX_DMA_CHANNEL;
  RTCMIF_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  RTCMIF_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  RTCMIF_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  RTCMIF_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  RTCMIF_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  RTCMIF_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  RTCMIF_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&RTCMIF_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, RTCMIF_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (USART3_TX) */
  HAL_NVIC_SetPriority(RTCM_UART_DMA_TX_IRQn, INT_PRIO_RTCMIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(RTCM_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (USART3_RX) */
  HAL_NVIC_SetPriority(RTCM_UART_DMA_RX_IRQn, INT_PRIO_RTCMIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(RTCM_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(RTCM_UART_IRQn, INT_PRIO_RTCMIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(RTCM_UART_IRQn);
}

void rtcmif_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  RTCM_UART_FORCE_RESET();
  RTCM_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure RTCM_UART Tx as alternate function  */
  HAL_GPIO_DeInit(RTCM_UART_TX_GPIO_PORT, RTCM_UART_TX_PIN);
  /* Configure RTCM_UART Rx as alternate function  */
  HAL_GPIO_DeInit(RTCM_UART_RX_GPIO_PORT, RTCM_UART_RX_PIN);

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
  HAL_NVIC_DisableIRQ(RTCM_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(RTCM_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void RTCM_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(RTCMIF_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void RTCM_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(RTCMIF_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void RTCM_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&RTCMIF_UartHandle);
  HAL_UART_IRQHandler(&RTCMIF_UartHandle);
}
