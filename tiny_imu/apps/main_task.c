/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"

//USB_CORE_HANDLE USB_Device_dev;

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * arg)
{
	/* The Application layer has only to call USBD_Init to
	initialize the USB low level driver, the USB device library, the USB clock
	,pins and interrupt service routine (BSP) to start the Library*/
//	USBD_Init(&USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);

	_delay_ms(50);
	uart2_init(kyLink_DecodeProcess);
	sEE_Init();
	mpu9250_init();
	_delay_ms(10);

	for(;;) {
		com_tx_task();
	}
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  uart2_TxBytesDMA((uint8_t *)"_HARD\n", 6);
	  _delay_ms(200);
  }
}

uint32_t sEE_TIMEOUT_UserCallback(void)
{
	uart2_TxBytesDMA((uint8_t *)"IIC ERR.\n", 9);
	return 0;
}
