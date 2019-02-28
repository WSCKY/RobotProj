/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"
#include "SysConfig.h"

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * argument)
{
	uart2_init();
	kyLink_Init();

	osDelay(50);
	mpu9250_init();

	osDelay(100);
	uart2_TxBytesDMA((uint8_t *)"IMU ATT EST START\n", 18);
	osDelay(100);
	for(;;) {
//		if(mpu_pull_new(&raw)) {
//			uart2_TxBytes((uint8_t *)"GOT NEW\n", 8);
//			osDleay(200);
//		}

		osDelay(200);
		uart2_TxBytesDMA((uint8_t *)"running ... /\r\0", 15);
		osDelay(200);
		uart2_TxBytesDMA((uint8_t *)"running ... -\r\0", 15);
		osDelay(200);
		uart2_TxBytesDMA((uint8_t *)"running ... \\\r\0", 15);
		osDelay(200);
		uart2_TxBytesDMA((uint8_t *)"running ... |\r\0", 15);
		osDelay(200);
	}
	/* Delete the Init Thread */
	    //osThreadTerminate(NULL);
}
