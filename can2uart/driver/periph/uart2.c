/*
 * uart2.c
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#include "uart2.h"

static uint8_t _uart2_init_flag = 0;
#if UART2_DMA_ENABLE
static uint32_t _tx_comp_flag = 1;
static DMA_InitTypeDef DMA_InitStructure;

static uint32_t in_ptr = 0, out_ptr = 0;
extern uint8_t UART_RX_CACHE[UART_RX_CACHE_SIZE];

static void dma_config(void);
#else
static PortRecvByteCallback pCallback = 0;
#endif /* UART2_DMA_ENABLE */

void uart2_init(
#if UART2_DMA_ENABLE
		void
#else
		PortRecvByteCallback p
#endif
		)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	if(_uart2_init_flag == 1) return; // already init.
#if !UART2_DMA_ENABLE
	if(p != 0) {
		pCallback = p;
	}
#endif
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(UART2_GPIO_CLK, ENABLE);

	/* Connect pin to Periph */
	GPIO_PinAFConfig(UART2_GPIO, UART2_TxPin_AF, UART2_AF);
	GPIO_PinAFConfig(UART2_GPIO, UART2_RxPin_AF, UART2_AF);

	/* Configure UART2 pins as AF pushpull */
	GPIO_InitStructure.GPIO_Pin = UART2_RxPin | UART2_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(UART2_GPIO, &GPIO_InitStructure);

	USART_DeInit(UART2);
	USART_Cmd(UART2, DISABLE);
	USART_ITConfig(UART2, USART_IT_RXNE, DISABLE);

	/* Enable UART2 Clock */
	UART2_CLK_CMD(UART2_CLK, ENABLE);

#if UART2_DMA_ENABLE
	dma_config();
#endif /* UART2_DMA_ENABLE */
	/* UART2 configuration -------------------------------------------*/
  /* UART2 configured as follow:
        - BaudRate = 115200
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive && Transmit enabled
  */

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	/* Configure UART2 */
	USART_Init(UART2, &USART_InitStructure);

	/* Enable the UART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = UART2_RX_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

#if UART2_DMA_ENABLE
	/* Enable the UART2 Idle line detection interrupt. */
	USART_ITConfig(UART2, USART_IT_IDLE, ENABLE);
	/* Enable the UART2 Tx DMA requests */
	USART_DMACmd(UART2, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
#else
	/* Enable the UART2 Receive Interrupt */
	USART_ITConfig(UART2, USART_IT_RXNE, ENABLE);
#endif /* UART2_DMA_ENABLE */
	/* Enable UART2 */
	USART_Cmd(UART2, ENABLE);

	_uart2_init_flag = 1;
}
#if UART2_DMA_ENABLE
static void dma_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable UART2_DMA Clock */
	UART2_DMA_CLK_CMD(UART2_DMA_CLK, ENABLE);

	DMA_DeInit(UART2_TX_DMA);
	DMA_Cmd(UART2_TX_DMA, DISABLE);
	DMA_DeInit(UART2_RX_DMA);
	DMA_Cmd(UART2_RX_DMA, DISABLE);

	/* UART2_RX_DMA configuration */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_InitStructure.DMA_BufferSize = UART_RX_CACHE_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART_RX_CACHE;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART2->RDR);
	DMA_Init(UART2_RX_DMA, &DMA_InitStructure);

	/* UART2_TX_DMA configuration */
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART2->TDR);
	DMA_Init(UART2_TX_DMA, &DMA_InitStructure);

	/* Enable the UART2_TX_DMA TC Interrupt */
	DMA_ITConfig(UART2_TX_DMA, DMA_IT_TC, ENABLE);
	/* Enable the UART2_RX_DMA HT/TC Interrupt */
	DMA_ITConfig(UART2_RX_DMA, DMA_IT_HT, ENABLE);
	DMA_ITConfig(UART2_RX_DMA, DMA_IT_TC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = UART2_DMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = UART2_DMA_INT_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable the UART2_TX_DMA channels */
//	DMA_Cmd(UART2_TX_DMA, ENABLE);
	/* Enable the UART2_RX_DMA channels */
	DMA_Cmd(UART2_RX_DMA, ENABLE);
}
#endif /* UART2_DMA_ENABLE */
void uart2_TxByte(uint8_t c)
{
	UART2->TDR = (c & (uint16_t)0x01FF);
	while(USART_GetFlagStatus(UART2, USART_FLAG_TXE) == RESET) {}
}

void uart2_TxBytes(uint8_t *p, uint32_t l)
{
	while(l --) {
		uart2_TxByte(*p ++);
	}
}
#if UART2_DMA_ENABLE
void uart2_TxBytesDMA(uint8_t *p, uint32_t l)
{
	if(_tx_comp_flag == 1) {
		_tx_comp_flag = 0;
		DMA_InitStructure.DMA_BufferSize = l;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)p;
		DMA_Init(UART2_TX_DMA, &DMA_InitStructure);

		/* Enable the UART2_DMA channels */
		DMA_Cmd(UART2_TX_DMA, ENABLE);
	}
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void uart2_flush(void)
{
	while(_tx_comp_flag == 0) {}
}
#pragma GCC pop_options

uint8_t uart2_pullByte(uint8_t *p)
{
	if(out_ptr != in_ptr) {
		*p = UART_RX_CACHE[out_ptr];
		if(++ out_ptr == UART_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
		return 1;
	}
	return 0;
}

uint32_t uart2_pullBytes(uint8_t *p, uint32_t l)
{
	uint32_t len = 0, cnt;
	if(out_ptr == in_ptr) return len;
	if(in_ptr > out_ptr) {
		len = in_ptr - out_ptr;
	} else {
		len = UART_RX_CACHE_SIZE - out_ptr + in_ptr;
	}
	if(len > l) len = l;
	cnt = len;
	while(cnt --) {
		*p = UART_RX_CACHE[out_ptr];
		p ++;
		if(++ out_ptr == UART_RX_CACHE_SIZE) {
			out_ptr = 0;
		}
	}
	return len;
}

static void uart2_pushBytes(void)
{
	/* Calculate current position in buffer */
	in_ptr = UART_RX_CACHE_SIZE - UART2_RX_DMA->CNDTR;
}
#else
void uart2_set_callback(PortRecvByteCallback p)
{
	if(p != 0) {
		pCallback = p;
	}
}
#endif /* UART2_DMA_ENABLE */

#if UART2_DMA_ENABLE
void UART2_DMA_IRQHandler(void)
{
	/* check if transfer complete flag is set. */
	if(DMA_GetITStatus(UART2_TX_DMA_IT_TC_FLAG)) {
		DMA_Cmd(UART2_TX_DMA, DISABLE);
		_tx_comp_flag = 1;
		DMA_ClearITPendingBit(UART2_TX_DMA_IT_TC_FLAG);
	}
	/* check if receive half complete flag is set. */
	if(DMA_GetITStatus(UART2_RX_DMA_IT_HT_FLAG)) {
		uart2_pushBytes();
		DMA_ClearITPendingBit(UART2_RX_DMA_IT_HT_FLAG);
	}
	/* check if receive complete flag is set. */
	if(DMA_GetITStatus(UART2_RX_DMA_IT_TC_FLAG)) {
		uart2_pushBytes();
		DMA_ClearITPendingBit(UART2_RX_DMA_IT_TC_FLAG);
	}
}
#endif /* UART2_DMA_ENABLE */
void UART2_IRQHandler(void)
{
#if UART2_DMA_ENABLE
	if(USART_GetITStatus(UART2, USART_IT_IDLE) != RESET) {
		uart2_pushBytes();
		USART_ClearFlag(UART2, USART_FLAG_IDLE);
		USART_ClearITPendingBit(UART2, USART_IT_IDLE);
	}
#else
	USART_ClearFlag(UART2, USART_FLAG_ORE | USART_FLAG_PE);
	if(USART_GetITStatus(UART2, USART_IT_RXNE) != RESET) {
		if(pCallback != 0) {
			pCallback(UART2->RDR & 0xFF);
		}
		USART_ClearFlag(UART2, USART_FLAG_RXNE);
		USART_ClearITPendingBit(UART2, USART_IT_RXNE);
	}
#endif /* UART2_DMA_ENABLE */
}
