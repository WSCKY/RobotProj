/**
  ******************************************************************************
  * @file    ./main.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static CanRxMsg *pRxMessage = 0;

static _FLOAT_UNION _tof_data = {0}, _ult_data = {0};
static uint32_t LastTime = 0, CurrentTime = 0;

#if MONITOR_TYPE_TERMINAL
#include <stdio.h>
#else
static kyLinkPackageDef ReportData;
static KYLINK_CORE_HANDLE UART2_PORT_HANDLE;
#endif /* MONITOR_TYPE_TERMINAL */

/* Private function prototypes -----------------------------------------------*/
#if MONITOR_TYPE_TERMINAL
static void printID(uint32_t id);
static void printDist(float d);
#endif /* MONITOR_TYPE_TERMINAL */
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void StartThread(void const * arg)
{
	LED_Init(); LED_R_ON(); LED_B_OFF();

	CAN_If_Init();

	uart2_init();
	kyLinkInit(&UART2_PORT_HANDLE, uart2_TxBytesDMA);

	pRxMessage = GetCAN_RxMsg();

	CurrentTime = _Get_Micros();
	LastTime = CurrentTime;

	kyLinkInitPackage(&ReportData);
	ReportData.FormatData.msg_id = TYPE_CAN_DATA_Report;
	ReportData.FormatData.length = sizeof(CAN_NODE_MSG_DEF);

	kyLinkTxEnable(&UART2_PORT_HANDLE);

	for(;;) {
		CurrentTime = _Get_Micros();
		if(GetMsgUpdateFlag()) {
			_tof_data.uData[0] = pRxMessage->Data[0];
			_tof_data.uData[1] = pRxMessage->Data[1];
			_tof_data.uData[2] = pRxMessage->Data[2];
			_tof_data.uData[3] = pRxMessage->Data[3];
			_ult_data.uData[0] = pRxMessage->Data[4];
			_ult_data.uData[1] = pRxMessage->Data[5];
			_ult_data.uData[2] = pRxMessage->Data[6];
			_ult_data.uData[3] = pRxMessage->Data[7];
#if MONITOR_TYPE_TERMINAL
			uart2_TxBytesDMA((uint8_t *)"id(", 3); uart2_flush();
			printID(pRxMessage->StdId);
			uart2_TxBytesDMA((uint8_t *)"): ", 3); uart2_flush();
			uart2_TxBytesDMA((uint8_t *)"tof: ", 5); uart2_flush();
			printDist(_tof_data.fData);
			uart2_TxBytesDMA((uint8_t *)"cm, usc: ", 9); uart2_flush();
			printDist(_ult_data.fData);
			uart2_TxBytesDMA((uint8_t *)"cm                 \r", 20); uart2_flush();
#else
			ReportData.FormatData.PacketData.TypeData.NodeData.NodeID = pRxMessage->StdId;
			ReportData.FormatData.PacketData.TypeData.NodeData.NodeData1 = _tof_data.fData;
			ReportData.FormatData.PacketData.TypeData.NodeData.NodeData2 = _ult_data.fData;
			SendTxPacket(&UART2_PORT_HANDLE, &ReportData);
#endif /* MONITOR_TYPE_TERMINAL */
			LED_B_TOG();
			LED_R_ON();
			LastTime = CurrentTime;
		}

		if((CurrentTime - LastTime) > 500000) {
			LED_R_TOG();
#if MONITOR_TYPE_TERMINAL
			uart2_TxBytesDMA((uint8_t *)"\e[0;31mLOST.\n\e[0m", 17); uart2_flush();
#endif /* MONITOR_TYPE_TERMINAL */
			LastTime = CurrentTime;
		}
	}
}
#if MONITOR_TYPE_TERMINAL
int __io_putchar(int ch)
{
	LOG_PORT_PUT_CHAR(ch);
	return ch;
}

static void printID(uint32_t id)
{
	int c = 0;
	if(id < 0x5A0) {
		uart2_TxBytes((uint8_t *)"??", 2);
	} else {
		id -= 0x5A0;
		c = id;
		if(id >= 100) {
			LOG_PORT_PUT_CHAR('?');
			c %= 100;
		}
		if(id >= 10) {
			LOG_PORT_PUT_CHAR('0' + c / 10);
			c %= 10;
		}
		LOG_PORT_PUT_CHAR('0' + c);
	}
}

static void printDist(float d)
{
	int c = d;
	if(d >= 1000) {
		LOG_PORT_PUT_CHAR('?');
		c %= 1000;
	}
	if(d >= 100) {
		LOG_PORT_PUT_CHAR('0' + c / 100);
		c = c % 100;
	}
	if(d >= 10) {
		LOG_PORT_PUT_CHAR('0' + c / 10);
		c = c % 10;
	}
	LOG_PORT_PUT_CHAR('0' + c);

}
#endif /* MONITOR_TYPE_TERMINAL */

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************************** END OF FILE *********************************/
