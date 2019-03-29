/**
  ******************************************************************************
  * @file    ./src/CAN.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Configure MCU CAN Peripheral.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "CAN.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t _rx_flag = 0;
static CanRxMsg _rx_msg = {0}, RxMessage = {0};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  set CAN interface initialization.
  * @param  None
  * @retval None
  */
void CAN_If_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_InitTypeDef       CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	/* CAN GPIOs configuration **************************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	CAN_GPIO_REMAP();

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(CAN_GPIO_CLK, ENABLE);

	/* Connect CAN pins to AF4 */
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_RX_SOURCE, CAN_AF_PORT);
	GPIO_PinAFConfig(CAN_GPIO_PORT, CAN_TX_SOURCE, CAN_AF_PORT); 

	/* Configure CAN RX and TX pins */
	GPIO_InitStructure.GPIO_Pin = CAN_RX_PIN | CAN_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(CAN_GPIO_PORT, &GPIO_InitStructure);

	/* NVIC configuration *******************************************************/
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* CAN configuration ********************************************************/  
	/* Enable CAN clock */
	RCC_APB1PeriphClockCmd(CAN_CLK, ENABLE);

	/* CAN register init */
	CAN_DeInit(CANx);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;

	/* CAN Baudrate = 500KBps (CAN clocked at 48 MHz) */
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_2tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 16;
	CAN_Init(CANx, &CAN_InitStructure);

	/* CAN filter init "FIFO0" */
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(CANx, CAN_IT_FMP0, ENABLE);
}

/**
  * @brief  Transmit a package of data through CAN.
  * @param  pTxMessage: pointer to a TxMessage to send.
  * @retval None
  */
void CAN_TransmitData(CanTxMsg* pTxMessage)
{
	CAN_Transmit(CANx, pTxMessage);
}

/**
  * @brief  Check if received a new message.
  * @param  None
  * @retval RxMessage update flag.
  */
uint8_t GetMsgUpdateFlag(void)
{
	if(_rx_flag) { _rx_flag = 0; return 1; } return 0;
}

/**
  * @brief  Get RxMessage data address.
  * @param  None
  * @retval message pointer.
  */
CanRxMsg* GetCAN_RxMsg(void)
{
	return &RxMessage;
}

/**
	* @brief  This function handles CAN request.
	* @param  None
	* @retval None
	*/
void CEC_CAN_IRQHandler(void)
{
  if (CAN_MessagePending(CANx, CAN_FIFO0) != 0) {
    CAN_Receive(CANx, CAN_FIFO0, &_rx_msg);
    RxMessage = _rx_msg;
    _rx_flag = 1;
  }
}

/******************************** END OF FILE *********************************/
