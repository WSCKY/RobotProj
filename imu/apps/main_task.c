/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"
#include "SysConfig.h"

osThreadId IMU_ThreadId;
//osThreadId COM_ThreadId;

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * argument)
{
	uart2_init(0);
	osDelay(50);
	mpu9250_init();

//	osDelay(100);
//	uart2_TxBytesDMA((uint8_t *)"IMU ATT EST START\n", 18);
//	osDelay(100);

	QueueHandle_t* q_mpu_raw = mpu_queue_get();
	osThreadDef(CAL, CalibTask, osPriorityHigh, 0, configMINIMAL_STACK_SIZE);
	osThreadCreate(osThread(CAL), q_mpu_raw);
	/* Thread definition */
//	osThreadDef(IMU, IMU_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//	osThreadDef(COM, COM_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
	/* Start thread */
//	IMU_ThreadId = osThreadCreate(osThread(IMU), NULL);
//	COM_ThreadId = osThreadCreate(osThread(COM), NULL);

	/* Delete the Init Thread */
	osThreadTerminate(NULL);
	for(;;);

//	for(;;) {
////		if(mpu_pull_new(&raw)) {
////			uart2_TxBytes((uint8_t *)"GOT NEW\n", 8);
////			osDleay(200);
////		}
//
//		osDelay(200);
//		uart2_TxBytesDMA((uint8_t *)"running ... /\r\0", 15);
//		osDelay(200);
//		uart2_TxBytesDMA((uint8_t *)"running ... -\r\0", 15);
//		osDelay(200);
//		uart2_TxBytesDMA((uint8_t *)"running ... \\\r\0", 15);
//		osDelay(200);
//		uart2_TxBytesDMA((uint8_t *)"running ... |\r\0", 15);
//		osDelay(200);
//	}
}
