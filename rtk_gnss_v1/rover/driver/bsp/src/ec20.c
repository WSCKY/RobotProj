#include "ec20.h"
#include "string.h"

#include "ringbuffer.h"

/* UART handler declaration */
static UART_HandleTypeDef EC20_UartHandle;
static DMA_HandleTypeDef EC20_TxDmaHandle;
static DMA_HandleTypeDef EC20_RxDmaHandle;

static __IO bool_t _ec20if_tx_ready = true;

#define EC20_RB_CAPACITY                           1024
static ringbuffer_handle ec20_rb;
static uint8_t ringbuffer_cache[EC20_RB_CAPACITY];

//static uint8_t ec20if_cache[EC20IF_CACHE_DEPTH];
static uint8_t *ec20if_cache = NULL;
//static uint32_t _ec20_data_available = 0, _ec20_ptr_out = 0;

status_t ec20if_init(void)
{
  /*##-1- Configure the UART peripheral ######################################*/
  /* Put the USART peripheral in the Asynchronous mode (UART Mode) */
  /* UART configured as follows:
      - Word Length = 8 Bits
      - Stop Bit = One Stop bit
      - Parity = None
      - BaudRate = specific baud
      - Hardware flow control disabled (RTS and CTS signals) */
  EC20_UartHandle.Instance        = EC20_UART;

  EC20_UartHandle.Init.BaudRate   = 115200;
  EC20_UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  EC20_UartHandle.Init.StopBits   = UART_STOPBITS_1;
  EC20_UartHandle.Init.Parity     = UART_PARITY_NONE;
  EC20_UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  EC20_UartHandle.Init.Mode       = UART_MODE_TX_RX;
  if(HAL_UART_DeInit(&EC20_UartHandle) != HAL_OK) return status_error;
  if(HAL_UART_Init(&EC20_UartHandle) != HAL_OK) return status_error;

  ringbuffer_init(&ec20_rb, ringbuffer_cache, EC20_RB_CAPACITY);

//  memset(ec20if_cache, 0x00, EC20IF_CACHE_DEPTH);
  ec20if_cache = kmm_alloc(EC20IF_CACHE_DEPTH);
  if(ec20if_cache == NULL) return status_nomem;
//  SCB_CleanDCache();
  __HAL_UART_ENABLE_IT(&EC20_UartHandle, UART_IT_IDLE);
  HAL_UART_Receive_DMA(&EC20_UartHandle, (uint8_t *)ec20if_cache, EC20IF_CACHE_DEPTH);
  return status_ok;
}

void ec20_power_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  EC20_Delay(200); // wait for power stabled.
  /* -1- Enable GPIO Clock (to be able to program the configuration registers) */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* -2- Configure IO in output push-pull mode to drive external Pins */
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // PC0 -> DTR
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // PC2 -> RESET
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct); // PC3 -> POWER

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); // DTR -> LOW
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // RESET -> LOW
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);
  EC20_Delay(200);
}

void ec20_power_on(void)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);   // PWR -> HIGH
  EC20_Delay(300);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // PWR -> LOW (release)
  EC20_Delay(200);
}

void ec20_power_off(void)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET);   // PWR -> HIGH
  EC20_Delay(600);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // PWR -> LOW (release)
  EC20_Delay(200);
}

static status_t cmp_ack(uint8_t *pSrc, uint32_t len)
{
  uint8_t rdata;
  uint32_t cnt = 0;
  while(len --) {
    if(ringbuffer_check_byte_offset(&ec20_rb, &rdata, cnt) != status_ok) {
      ringbuffer_reset(&ec20_rb);
      return status_error;
    }
    if(*pSrc != rdata) {
      ringbuffer_poll_byte(&ec20_rb, &rdata);
      return status_error;
    }
    pSrc ++;
    cnt ++;
  }
  ringbuffer_reset(&ec20_rb);
  return status_ok;
}

status_t ec20_check_ack(uint8_t *p, uint32_t timeout)
{
  uint32_t _tick_end = EC20_Ticks() + timeout;
  uint32_t size = strlen((const char *)p);
  while(_tick_end > EC20_Ticks()) {
    while(ringbuffer_available(&ec20_rb) >= size) {
      if(cmp_ack(p, size) == status_ok)
        return status_ok;
    }
    EC20_Delay(20);
  }
  return status_timeout;
}

status_t ec20_check_cmd_ack(uint8_t *cmd, uint8_t *ack, uint32_t tries, uint32_t timeout)
{
  while(tries --) {
    ec20if_tx_bytes_util(cmd, strlen((const char *)cmd));
    if(ec20_check_ack(ack, timeout) == status_ok) {
      return status_ok;
    }
  }
  return status_error;
}

status_t ec20if_tx_string_util(const char *str)
{
  return ec20if_tx_bytes_util((uint8_t *)str, strlen(str));
}

status_t ec20if_tx_bytes(uint8_t *p, uint32_t l)
{
  return (status_t)HAL_UART_Transmit_DMA(&EC20_UartHandle, (uint8_t*)p, l);
}

status_t ec20if_tx_bytes_util(uint8_t *p, uint32_t l)
{
  status_t ret = status_ok;
  if(l == 0) return ret;
  while(_ec20if_tx_ready == false) {}
  _ec20if_tx_ready = false;
  ret = (status_t)HAL_UART_Transmit_DMA(&EC20_UartHandle, p, l);
  while(_ec20if_tx_ready == false) {}
  return ret;
}

uint32_t ec20if_rx_bytes(uint8_t *p, uint32_t l)
{
//  uint32_t len = l;
//  if(_ec20_data_available < l) len = _ec20_data_available;
//  for(uint32_t cnt = 0; cnt < len; cnt ++) {
//    p[cnt] = ec20if_cache[_ec20_ptr_out ++];
//    _ec20_data_available --;
//    if(_ec20_ptr_out == EC20IF_CACHE_DEPTH) _ec20_ptr_out = 0;
//  }
//  return len;
  return ringbuffer_poll(&ec20_rb, p, l);
}

uint32_t ec20if_cache_usage(void)
{
  return ringbuffer_usage(&ec20_rb);
}

void ec20if_txcplt_callback(UART_HandleTypeDef *huart)
{
  _ec20if_tx_ready = true;
}

void ec20if_rxcplt_callback(UART_HandleTypeDef *huart)
{
}
uint32_t max_remain = 0;
uint32_t idle_receive = 0;
static void UART_IDLE_Callback(UART_HandleTypeDef *huart)
{
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    __HAL_UART_CLEAR_IT(huart, UART_CLEAR_IDLEF);

//    uint32_t current_pos = EC20IF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
//    if(current_pos >= _ec20_ptr_out)
//      _ec20_data_available = current_pos - _ec20_ptr_out;
//    else
//      _ec20_data_available = current_pos + EC20IF_CACHE_DEPTH - _ec20_ptr_out;
    uint32_t len = EC20IF_CACHE_DEPTH - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(len > max_remain) max_remain = len;
    idle_receive = len;
    if(len > 0) {
      __HAL_DMA_DISABLE(huart->hdmarx);
      ringbuffer_push(&ec20_rb, ec20if_cache, len);
      __HAL_DMA_SET_COUNTER(huart->hdmarx, EC20IF_CACHE_DEPTH);
      __HAL_DMA_ENABLE(huart->hdmarx);
//      __HAL_DMA_CLEAR_FLAG(huart->hdmarx, DMA_FLAG_TCIF0_4);
    }
  }
}

void ec20if_msp_init(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* Enable GPIO TX/RX clock */
  EC20_UART_TX_GPIO_CLK_ENABLE();
  EC20_UART_RX_GPIO_CLK_ENABLE();

  /* Enable EC20_UART clock */
  EC20_UART_CLK_ENABLE();

  /* Enable DMA clock */
  EC20_UART_DMA_CLK_ENABLE();

  /*##-2- Configure peripheral GPIO ##########################################*/  
  /* UART TX GPIO pin configuration  */
  GPIO_InitStruct.Pin       = EC20_UART_TX_PIN;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = EC20_UART_TX_AF;

  HAL_GPIO_Init(EC20_UART_TX_GPIO_PORT, &GPIO_InitStruct);

  /* UART RX GPIO pin configuration  */
  GPIO_InitStruct.Pin = EC20_UART_RX_PIN;
  GPIO_InitStruct.Alternate = EC20_UART_RX_AF;

  HAL_GPIO_Init(EC20_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  /*##-3- Configure the DMA ##################################################*/
  /* Configure the DMA handler for Transmission process */
  EC20_TxDmaHandle.Instance                 = EC20_UART_TX_DMA_STREAM;
  EC20_TxDmaHandle.Init.Channel             = EC20_UART_TX_DMA_CHANNEL;
  EC20_TxDmaHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  EC20_TxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  EC20_TxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  EC20_TxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  EC20_TxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  EC20_TxDmaHandle.Init.Mode                = DMA_NORMAL;
  EC20_TxDmaHandle.Init.Priority            = DMA_PRIORITY_LOW;

  HAL_DMA_Init(&EC20_TxDmaHandle);

  /* Associate the initialized DMA handle to the UART handle */
  __HAL_LINKDMA(huart, hdmatx, EC20_TxDmaHandle);

  /* Configure the DMA handler for reception process */
  EC20_RxDmaHandle.Instance                 = EC20_UART_RX_DMA_STREAM;
  EC20_RxDmaHandle.Init.Channel             = EC20_UART_RX_DMA_CHANNEL;
  EC20_RxDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  EC20_RxDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
  EC20_RxDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
  EC20_RxDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  EC20_RxDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  EC20_RxDmaHandle.Init.Mode                = DMA_CIRCULAR;//DMA_NORMAL;
  EC20_RxDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

  HAL_DMA_Init(&EC20_RxDmaHandle);

  /* Associate the initialized DMA handle to the the UART handle */
  __HAL_LINKDMA(huart, hdmarx, EC20_RxDmaHandle);

  /*##-4- Configure the NVIC for DMA #########################################*/
  /* NVIC configuration for DMA transfer complete interrupt (UART4_TX) */
  HAL_NVIC_SetPriority(EC20_UART_DMA_TX_IRQn, INT_PRIO_GSMIF_DMATX, 0);
  HAL_NVIC_EnableIRQ(EC20_UART_DMA_TX_IRQn);

  /* NVIC configuration for DMA transfer complete interrupt (UART4_RX) */
  HAL_NVIC_SetPriority(EC20_UART_DMA_RX_IRQn, INT_PRIO_GSMIF_DMARX, 0);
  HAL_NVIC_EnableIRQ(EC20_UART_DMA_RX_IRQn);

  /* NVIC for USART, to catch the TX complete */
  HAL_NVIC_SetPriority(EC20_UART_IRQn, INT_PRIO_GSMIF_PERIPH, 0);
  HAL_NVIC_EnableIRQ(EC20_UART_IRQn);
}

void ec20if_msp_deinit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  EC20_UART_FORCE_RESET();
  EC20_UART_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure EC20_UART Tx as alternate function  */
  HAL_GPIO_DeInit(EC20_UART_TX_GPIO_PORT, EC20_UART_TX_PIN);
  /* Configure EC20_UART Rx as alternate function  */
  HAL_GPIO_DeInit(EC20_UART_RX_GPIO_PORT, EC20_UART_RX_PIN);

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
  HAL_NVIC_DisableIRQ(EC20_UART_DMA_TX_IRQn);
  HAL_NVIC_DisableIRQ(EC20_UART_DMA_RX_IRQn);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void EC20_UART_DMA_RX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(EC20_UartHandle.hdmarx);
}

/**
  * @brief  This function handles DMA interrupt request.
  * @param  None
  * @retval None
  */
void EC20_UART_DMA_TX_IRQHandler(void)
{
  HAL_DMA_IRQHandler(EC20_UartHandle.hdmatx);
}

/**
  * @brief  This function handles UART interrupt request.
  * @param  None
  * @retval None
  */
void EC20_UART_IRQHandler(void)
{
  UART_IDLE_Callback(&EC20_UartHandle);
  HAL_UART_IRQHandler(&EC20_UartHandle);
}
