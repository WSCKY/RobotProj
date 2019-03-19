/*
 * acc_calib.c
 *
 *  Created on: Mar 14, 2019
 *      Author: kychu
 */

#include "acc_calib.h"

static pthread_t calib_thread;
static pthread_t check_thread;
static pthread_t recvr_thread;
static pthread_t exit_thread;
static void calib_task(void);
static void check_task(void);
static void recvr_task(void);
static void exit_task(void);

static int start_flag = 0;
static int exit_flag = 0;

int acc_calib_start(void)
{
	if(pthread_create(&calib_thread, NULL, (void *)calib_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&check_thread, NULL, (void *)check_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&recvr_thread, NULL, (void *)recvr_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&exit_thread, NULL, (void *)exit_task, NULL) != 0)
		return EXIT_FAILURE;
	start_flag = 1;
	return EXIT_SUCCESS;
}

void acc_calib_wait_exit(void)
{
	pthread_join(calib_thread, NULL);
	pthread_join(check_thread, NULL);
	pthread_join(recvr_thread, NULL);
	pthread_join(exit_thread, NULL);
}

extern KYLINK_CORE_HANDLE kyLinkHandle;
static kyLinkPackageDef txPacket = {0};

static uint8_t CalibStartFlag = 0;

static void calib_task(void)
{
	reset_matrix();
	kyLinkInitPackage(&txPacket);
//	txPacket.FormatData.msg_id = TYPE_UPPER_CALIB_REQ;
//	txPacket.FormatData.length = sizeof(UpperCalibCmdDef);
//	txPacket.FormatData.PacketData.TypeData.CalibCmd.Mode = 0x0;
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		if(CalibStartFlag == 0) {

		}
		usleep(2000);
	}
}

static kyLinkPackageDef *pRx;
static void recvr_task(void)
{
	pRx = GetRxPackage(&kyLinkHandle);
	while(start_flag == 0) {}
	printf("|     gx     |     gy     |     gz     |     ax     |     ay     |     az     |\n");
	printf("+------------+------------+------------+------------+------------+------------+\n");
	while(exit_flag == 0) {
		if(kyLinkCheckUpdate(&kyLinkHandle) == kyTRUE) {
			if(pRx->FormatData.msg_id == TYPE_IMU_INFO_Resp) {
				printf("|%-12f|%-12f|%-12f|%-12f|%-12f|%-12f|\r",
                       pRx->FormatData.PacketData.TypeData.IMU_InfoData.gyrX,
					   pRx->FormatData.PacketData.TypeData.IMU_InfoData.gyrY,
					   pRx->FormatData.PacketData.TypeData.IMU_InfoData.gyrZ,
					   pRx->FormatData.PacketData.TypeData.IMU_InfoData.accX,
					   pRx->FormatData.PacketData.TypeData.IMU_InfoData.accY,
					   pRx->FormatData.PacketData.TypeData.IMU_InfoData.accZ);
				fflush(stdout);
			}
		}
		usleep(2000); // wait 2ms
	}
	printf("\n");
}

static void check_task(void)
{
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		usleep(2000);
	}
}

static void exit_task(void)
{
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		int v = getchar();
		switch(v) {
			case 0x1d: // detect for 'CTRL+['
				exit_flag = 1;
			break;
			default:
			break;
		}
	}
}
