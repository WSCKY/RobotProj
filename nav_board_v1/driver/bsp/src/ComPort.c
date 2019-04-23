#include "ComPort.h"

static uint32_t _tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

static PortRecvByteCallback pRecvCall = NULL;

void ComPort_Init(PortRecvByteCallback p)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

  if(p != NULL) { pRecvCall = p; }

/* GPIOx configuration -------------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(COM_PORT_TX_GPIO_CLK | COM_PORT_RX_GPIO_CLK, ENABLE);
	/* Enable UART clock */
	COM_PORT_CLK_INIT(COM_PORT_CLK, ENABLE);
	/* Enable the DMA clock */
  COM_PORT_DMAx_CLK_INIT(COM_PORT_DMAx_CLK, ENABLE);
  /* Connect GPIO_Pin to USARTx_Tx&USARTx_Rx */
  GPIO_PinAFConfig(COM_PORT_TX_GPIO_PORT, COM_PORT_TX_SOURCE, COM_PORT_TX_AF);
  GPIO_PinAFConfig(COM_PORT_RX_GPIO_PORT, COM_PORT_RX_SOURCE, COM_PORT_RX_AF);

  /* Configure USART Tx as alternate function  */
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  GPIO_InitStructure.GPIO_Pin = COM_PORT_TX_PIN;
  GPIO_Init(COM_PORT_TX_GPIO_PORT, &GPIO_InitStructure);
  /* Configure USART Rx as alternate function  */
  GPIO_InitStructure.GPIO_Pin = COM_PORT_RX_PIN;
  GPIO_Init(COM_PORT_RX_GPIO_PORT, &GPIO_InitStructure);

/* NVIC configuration --------------------------------------------------------*/
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = COM_PORT_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_COM_PORT_RX; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

/* Configure DMA controller to manage USART TX DMA request -------------------*/
	DMA_DeInit(COM_PORT_TX_DMA_STREAM);
	DMA_Cmd(COM_PORT_TX_DMA_STREAM, DISABLE);

	/* Configure DMA Initialization Structure */
	DMA_InitStructure.DMA_Channel = COM_PORT_TX_DMA_CHANNEL;	
	DMA_InitStructure.DMA_Memory0BaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;

	DMA_InitStructure.DMA_PeripheralBaseAddr = COM_PORT_DR_ADDRESS;
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
	DMA_Init(COM_PORT_TX_DMA_STREAM, &DMA_InitStructure);
	DMA_ITConfig(COM_PORT_TX_DMA_STREAM, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = COM_PORT_DMA_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_COM_PORT_TX_DMA; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

/* USARTx configuration ------------------------------------------------------*/
  /* USARTx configured as follows:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* USART configuration */
  USART_Init(COM_PORT, &USART_InitStructure);
	/* Enable Receive Data register not empty interrupt */
	USART_ITConfig(COM_PORT, USART_IT_RXNE, ENABLE);
	/* Enables the USART's DMA interface */
	USART_DMACmd(COM_PORT, USART_DMAReq_Tx, ENABLE);
  /* Enable USART */
  USART_Cmd(COM_PORT, ENABLE);
}

void ComPortCallbackRegistry(PortRecvByteCallback p)
{
  if(p != NULL) {
    pRecvCall = p;
  }
}

/*
 * Send any bytes through COM port use dma.
 */
void ComPortSendBytesDMA(uint8_t *p, uint32_t l)
{
	if(_tx_comp_flag == 1) {
		_tx_comp_flag = 0;

		DMA_InitStructure.DMA_BufferSize = l;
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)p;
		DMA_Init(COM_PORT_TX_DMA_STREAM, &DMA_InitStructure);
		/* Enable COM_USART DMA TX Stream */
		DMA_Cmd(COM_PORT_TX_DMA_STREAM, ENABLE);
	}
}

/*
 * this function handles COM port interrupt.
 */
void COM_PORT_IRQHandler(void)
{
  if(pRecvCall != NULL) {
    pRecvCall(COM_PORT->DR);
  }
	USART_ClearFlag(COM_PORT, USART_FLAG_RXNE);
	USART_GetFlagStatus(COM_PORT, USART_FLAG_ORE);
}

/*
 * this function handles UART DMA Interrupt.
 */
void COM_PORT_DMA_TX_IRQHandler(void)
{
	/* check if Streamx transfer complete flag is set. */
	if(DMA_GetFlagStatus(COM_PORT_TX_DMA_STREAM, COM_PORT_TX_DMA_FLAG_TCIF)) {
		DMA_Cmd(COM_PORT_TX_DMA_STREAM, DISABLE);
		_tx_comp_flag = 1;
		DMA_ClearFlag(COM_PORT_TX_DMA_STREAM, COM_PORT_TX_DMA_FLAG_TCIF);
	}
}
