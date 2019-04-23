#include "ubxUART.h"

static DMA_InitTypeDef DMA_InitStructure;
static USART_InitTypeDef USART_InitStructure;

#if FREERTOS_ENABLED
static SemaphoreHandle_t xSemaphore = NULL;
#else
static uint32_t _tx_comp_flag = 1;
#endif /* FREERTOS_ENABLED */

static void ubx_port_mutex_init(void);

void ubx_port_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  ubx_port_mutex_init();
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#endif /* FREERTOS_ENABLED */
/* GPIOx configuration -------------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(UBX_PORT_TX_GPIO_CLK | UBX_PORT_RX_GPIO_CLK, ENABLE);
	/* Enable UART clock */
	UBX_PORT_CLK_INIT(UBX_PORT_CLK, ENABLE);
	/* Enable the DMA clock */
  UBX_PORT_DMAx_CLK_INIT(UBX_PORT_DMAx_CLK, ENABLE);
  /* Connect GPIO_Pin to USARTx_Tx&USARTx_Rx */
  GPIO_PinAFConfig(UBX_PORT_TX_GPIO_PORT, UBX_PORT_TX_SOURCE, UBX_PORT_TX_AF);
  GPIO_PinAFConfig(UBX_PORT_RX_GPIO_PORT, UBX_PORT_RX_SOURCE, UBX_PORT_RX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

  GPIO_InitStructure.GPIO_Pin = UBX_PORT_TX_PIN;
  GPIO_Init(UBX_PORT_TX_GPIO_PORT, &GPIO_InitStructure);
  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Pin = UBX_PORT_RX_PIN;
  GPIO_Init(UBX_PORT_RX_GPIO_PORT, &GPIO_InitStructure);

/* NVIC configuration --------------------------------------------------------*/
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = UBX_PORT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UBX_PORT_RX; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/* Configure DMA controller to manage USART TX DMA request -------------------*/
	DMA_DeInit(UBX_PORT_TX_DMA_STREAM);
	DMA_Cmd(UBX_PORT_TX_DMA_STREAM, DISABLE);

	/* Configure DMA Initialization Structure */
	DMA_InitStructure.DMA_Channel = UBX_PORT_TX_DMA_CHANNEL;	
	DMA_InitStructure.DMA_Memory0BaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;

	DMA_InitStructure.DMA_PeripheralBaseAddr = UBX_PORT_DR_ADDRESS;
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
	DMA_Init(UBX_PORT_TX_DMA_STREAM, &DMA_InitStructure);
	DMA_ITConfig(UBX_PORT_TX_DMA_STREAM, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = UBX_PORT_DMA_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_UBX_PORT_TX_DMA; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 9600 baud
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx; /* default tx only */

  /* USART configuration */
  USART_Init(UBX_PORT, &USART_InitStructure);
	/* Disable Receive Data register not empty interrupt */
	USART_ITConfig(UBX_PORT, USART_IT_RXNE, DISABLE); /* RXNE default disbaled */
	/* Enables the USART's DMA interface */
	USART_DMACmd(UBX_PORT, USART_DMAReq_Tx, ENABLE);
  /* Enable USART */
  USART_Cmd(UBX_PORT, ENABLE);
#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#else
  _tx_comp_flag = 1;
#endif /* FREERTOS_ENABLED */
}

void ubx_port_config(uint32_t BaudRate, RX_STATUS RxEnable)
{
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#else
  _tx_comp_flag = 0;
#endif /* FREERTOS_ENABLED */
  USART_Cmd(UBX_PORT, DISABLE);
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_Mode = USART_Mode_Tx;
  if(RxEnable != RX_DISABLE) {
    USART_InitStructure.USART_Mode |= USART_Mode_Rx;
    USART_ITConfig(UBX_PORT, USART_IT_RXNE, ENABLE);
  } else {
    USART_ITConfig(UBX_PORT, USART_IT_RXNE, DISABLE);
  }
  USART_Init(UBX_PORT, &USART_InitStructure);
  USART_Cmd(UBX_PORT, ENABLE);
#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#else
  _tx_comp_flag = 1;
#endif /* FREERTOS_ENABLED */
}

static void ubx_port_mutex_init(void)
{
#if FREERTOS_ENABLED
  xSemaphore = xSemaphoreCreateMutex();
#else
  _tx_comp_flag = 0;
#endif /* FREERTOS_ENABLED */
}

void ubxPortSendBytes(uint8_t *p, uint32_t l)
{
#if FREERTOS_ENABLED
  xSemaphoreTake(xSemaphore, portMAX_DELAY);
#else
  _tx_comp_flag = 0;
#endif /* FREERTOS_ENABLED */
  while(l --) {
    UBX_PORT->DR = *p;
    p ++;
    while(USART_GetFlagStatus(UBX_PORT, USART_FLAG_TXE) == RESET);
  }
#if FREERTOS_ENABLED
  xSemaphoreGive(xSemaphore);
#else
  _tx_comp_flag = 1;
#endif /* FREERTOS_ENABLED */
}

/*
 * Send any bytes through ubx port use dma.
 */
void ubxPortSendBytesDMA(uint8_t *p, uint32_t l)
{
#if FREERTOS_ENABLED
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreTakeFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#else
  while(_tx_comp_flag == 0) {}
#endif /* FREERTOS_ENABLED */
		DMA_InitStructure.DMA_BufferSize = l;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)p;
		DMA_Init(UBX_PORT_TX_DMA_STREAM, &DMA_InitStructure);
		/* Enable UBX_USART DMA TX Stream */
		DMA_Cmd(UBX_PORT_TX_DMA_STREAM, ENABLE);
}

/*
 * this function handles ubx port interrupt.
 */
void UBX_PORT_IRQHandler(void)
{
	ubxPortRxDataCallback(UBX_PORT->DR);
	USART_ClearFlag(UBX_PORT, USART_FLAG_RXNE);
	USART_GetFlagStatus(UBX_PORT, USART_FLAG_ORE);
}

/*
 * this function handles UART DMA Interrupt.
 */
void UBX_PORT_DMA_TX_IRQHandler(void)
{
#if FREERTOS_ENABLED
  static BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
	/* check if Streamx transfer complete flag is set. */
	if(DMA_GetFlagStatus(UBX_PORT_TX_DMA_STREAM, UBX_PORT_TX_DMA_FLAG_TCIF)) {
		DMA_Cmd(UBX_PORT_TX_DMA_STREAM, DISABLE);
		DMA_ClearFlag(UBX_PORT_TX_DMA_STREAM, UBX_PORT_TX_DMA_FLAG_TCIF);
#if FREERTOS_ENABLED
    xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
#else
    _tx_comp_flag = 1;
#endif /* FREERTOS_ENABLED */
	}
}
