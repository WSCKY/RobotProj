#include "ComPort.h"

static uint8_t _com_port_init_flag = 0;
#if COM_PORT_DMA_ENABLE
static uint32_t _tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

static uint32_t in_ptr = 0, out_ptr = 0;
extern uint8_t UART2_RX_CACHE[UART2_RX_CACHE_SIZE];

static void dma_config(void);
static void ComPort_pushBytes(void);
#else
static PortRecvByteCallback pRecvCall = NULL;
#endif /* COM_PORT_DMA_ENABLE */

void ComPort_Init(
#if COM_PORT_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif /* COM_PORT_DMA_ENABLE */
		)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	if(_com_port_init_flag != 0) return; // already initialized.
#if !COM_PORT_DMA_ENABLE
	if(p != NULL) { pRecvCall = p; }
#endif /* !COM_PORT_DMA_ENABLE */
/* GPIOx configuration -------------------------------------------------------*/
	/* Enable GPIO clock */
	RCC_AHB1PeriphClockCmd(COM_PORT_TX_GPIO_CLK | COM_PORT_RX_GPIO_CLK, ENABLE);
	/* Enable UART clock */
	COM_PORT_CLK_INIT(COM_PORT_CLK, ENABLE);
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

#if COM_PORT_DMA_ENABLE
	dma_config();
#endif /* COM_PORT_DMA_ENABLE */

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
	USART_Init(COM_PORT, &USART_InitStructure);
#if COM_PORT_DMA_ENABLE
	/* Enable Idle line detection interrupt */
	USART_ITConfig(COM_PORT, USART_IT_IDLE, ENABLE);
	/* Enables the USART's DMA interface */
	USART_DMACmd(COM_PORT, USART_DMAReq_Rx | USART_DMAReq_Tx, ENABLE);
#else
	/* Enable Receive Data register not empty interrupt */
	USART_ITConfig(COM_PORT, USART_IT_RXNE, ENABLE);
#endif /* COM_PORT_DMA_ENABLE */
	/* Enable USART */
	USART_Cmd(COM_PORT, ENABLE);

	_com_port_init_flag = 1; // configured.
}

#if COM_PORT_DMA_ENABLE
static void dma_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the DMA clock */
	COM_PORT_DMAx_CLK_INIT(COM_PORT_DMAx_CLK, ENABLE);

	/* Configure DMA controller to manage USART TX/RX DMA request -------------------*/
	DMA_DeInit(COM_PORT_TX_DMA_STREAM);
	DMA_Cmd(COM_PORT_TX_DMA_STREAM, DISABLE);
	DMA_DeInit(COM_PORT_RX_DMA_STREAM);
	DMA_Cmd(COM_PORT_RX_DMA_STREAM, DISABLE);

	/* --------------- RX --------------- */
	NVIC_InitStructure.NVIC_IRQChannel = COM_PORT_DMA_RX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_COM_PORT_RX_DMA; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	/* Configure RX DMA Initialization Structure */
	DMA_InitStructure.DMA_Channel = COM_PORT_RX_DMA_CHANNEL;
	DMA_InitStructure.DMA_PeripheralBaseAddr = COM_PORT_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)UART2_RX_CACHE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = UART2_RX_CACHE_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(COM_PORT_RX_DMA_STREAM, &DMA_InitStructure);
	/* Enable DMA RX Half transfer complete interrupt && Transfer complete interrupt */
	DMA_ITConfig(COM_PORT_RX_DMA_STREAM, DMA_IT_HT, ENABLE);
	DMA_ITConfig(COM_PORT_RX_DMA_STREAM, DMA_IT_TC, ENABLE);

	/* --------------- TX --------------- */
	NVIC_InitStructure.NVIC_IRQChannel = COM_PORT_DMA_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_COM_PORT_TX_DMA; /* must >= configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, when use RTOS */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );

	/* Configure TX DMA Initialization Structure */
	DMA_InitStructure.DMA_Channel = COM_PORT_TX_DMA_CHANNEL;
	DMA_InitStructure.DMA_Memory0BaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = 0;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = COM_PORT_DR_ADDRESS;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc     = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize  = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize      = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode      = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority  = DMA_Priority_Medium;
//	DMA_InitStructure.DMA_FIFOMode      = DMA_FIFOMode_Enable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	DMA_InitStructure.DMA_MemoryBurst     = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(COM_PORT_TX_DMA_STREAM, &DMA_InitStructure);
	/* Enable DMA TX Complete Interrupt */
	DMA_ITConfig(COM_PORT_TX_DMA_STREAM, DMA_IT_TC, ENABLE);
}
#endif /* COM_PORT_DMA_ENABLE */

void ComPort_TxByte(uint8_t c)
{
	COM_PORT->DR = (c & (uint16_t)0x01FF);
	while(USART_GetFlagStatus(COM_PORT, USART_FLAG_TXE) == RESET) {}
}

void ComPort_TxBytes(uint8_t *p, uint32_t l)
{
	while(l --) {
		ComPort_TxByte(*p ++);
	}
}

#if COM_PORT_DMA_ENABLE
/*
 * Send any bytes through COM port use dma.
 */
void ComPort_TxBytesDMA(uint8_t *p, uint32_t l)
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

#pragma GCC push_options
#pragma GCC optimize ("O0")
void ComPort_flush(void)
{
	while(_tx_comp_flag == 0) {}
}
#pragma GCC pop_options

uint8_t ComPort_pullByte(uint8_t *p)
{
	if(out_ptr != in_ptr) {
		*p = UART2_RX_CACHE[out_ptr];
		if(++ out_ptr == UART2_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
		return 1;
	}
	return 0;
}

uint32_t ComPort_pullBytes(uint8_t *p, uint32_t l)
{
	uint32_t len = 0, cnt;
	if(out_ptr == in_ptr) return len;
	if(in_ptr > out_ptr) {
		len = in_ptr - out_ptr;
	} else {
		len = UART2_RX_CACHE_SIZE - out_ptr + in_ptr;
	}
	if(len > l) len = l;
	cnt = len;
	while(cnt --) {
		*p = UART2_RX_CACHE[out_ptr];
		p ++;
		if(++ out_ptr == UART2_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
	}
	return len;
}

static void ComPort_pushBytes(void)
{
	/* Calculate current position in buffer */
	in_ptr = UART2_RX_CACHE_SIZE - COM_PORT_RX_DMA_STREAM->NDTR;
}

#else
void ComPort_set_callback(PortRecvByteCallback p)
{
	if(p != NULL) pRecvCall = p;
}
#endif /* !COM_PORT_DMA_ENABLE */

/*
 * this function handles COM port interrupt.
 */
void COM_PORT_IRQHandler(void)
{
#if COM_PORT_DMA_ENABLE
	if(USART_GetITStatus(COM_PORT, USART_IT_IDLE) != RESET) {
		ComPort_pushBytes();
		USART_ClearITPendingBit(COM_PORT, USART_IT_IDLE);
	}
#else
	if(USART_GetITStatus(COM_PORT, USART_IT_RXNE) != RESET) {
		if(pRecvCall != NULL) {
			pRecvCall(COM_PORT->DR);
	}
		USART_ClearITPendingBit(COM_PORT, USART_IT_RXNE);
		USART_GetFlagStatus(COM_PORT, USART_FLAG_ORE);
	}
#endif /* COM_PORT_DMA_ENABLE */
}

#if COM_PORT_DMA_ENABLE
/*
 * this function handles UART DMA TX Interrupt.
 */
void COM_PORT_DMA_TX_IRQHandler(void)
{
	/* check if Streamx transfer complete flag is set. */
	if(DMA_GetITStatus(COM_PORT_TX_DMA_STREAM, COM_PORT_TX_DMA_IT_TCIF)) {
		DMA_Cmd(COM_PORT_TX_DMA_STREAM, DISABLE);
		_tx_comp_flag = 1;
		DMA_ClearITPendingBit(COM_PORT_TX_DMA_STREAM, COM_PORT_TX_DMA_IT_TCIF);
	}
}

/*
 * this function handles UART DMA RX Interrupt.
 */
void COM_PORT_DMA_RX_IRQHandler(void)
{
	/* check if Streamx half transfer flag is set. */
	if(DMA_GetITStatus(COM_PORT_RX_DMA_STREAM, COM_PORT_RX_DMA_IT_HTIF)) {
		ComPort_pushBytes();
		DMA_ClearITPendingBit(COM_PORT_RX_DMA_STREAM, COM_PORT_RX_DMA_IT_HTIF);
	}
	/* check if Streamx transfer complete flag is set. */
	if(DMA_GetITStatus(COM_PORT_RX_DMA_STREAM, COM_PORT_RX_DMA_IT_TCIF)) {
		ComPort_pushBytes();
		DMA_ClearITPendingBit(COM_PORT_RX_DMA_STREAM, COM_PORT_RX_DMA_IT_TCIF);
	}
}
#endif /* COM_PORT_DMA_ENABLE */
