/*
 * upgrade.c
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#include "upgrade.h"

typedef enum {
	Upgrade_SendReq = 0,
	Upgrade_SendDat = 1,

} UpgradeStep;

static pthread_t tx_thread;
static pthread_t rx_thread;
static pthread_t tim_thread;
static int start_flag = 0;
static int exit_flag = 0;

static long int ticks = 0;

static void timer_task(void);
static void upgrade_tx_task(void);
static void upgrade_rx_task(void);

int upgrade_start(void)
{
	if(pthread_create(&tim_thread, NULL, (void *)timer_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&tx_thread, NULL, (void *)upgrade_tx_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&rx_thread, NULL, (void *)upgrade_rx_task, NULL) != 0)
		return EXIT_FAILURE;
	start_flag = 1;
	return EXIT_SUCCESS;
}

void upgrade_wait_exit(void)
{
	pthread_join(tx_thread, NULL);
	pthread_join(rx_thread, NULL);
	pthread_join(tim_thread, NULL);
}

FW_INFO fw_head;
static CommPackageDef txPacket = {0};
static void upgrade_tx_task(void)
{
	long int time_a = 0;
	fw_head = fw_getInfo();
	InitCommPackage(&txPacket);
	int major = (fw_head.version & 0x00F0) >> 4;
	int minor = fw_head.version & 0x000F;
	int fixnum = (fw_head.version >> 8) & 0x00FF;
	printf("Size: %ldB, V%d.%d.%d, Type:%d @ %s\n", fw_head.size, major, minor, fixnum, fw_head.type, fw_head.time);
	while(start_flag == 0) {}
	printf("Start bootloader ...\n");

	while(exit_flag == 0) {
		printf("\n\e[0;31mWAIT ...\e[0m\n");
		sleep(2);
		exit_flag = 1;
	}
}

static CommPackageDef *pRx;
static void upgrade_rx_task(void)
{
	pRx = GetRxPacket();
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		if(GotNewData()) {

			printf("GOT DATA(0x%x:0x%x)\n", pRx->Packet.dev_id, pRx->Packet.msg_id);
		}
	}
}

static void timer_task(void)
{
	static long int now_ms = 0;
	static long int old_ms = 0;
	static struct timeval now_tv;

	gettimeofday(&now_tv, NULL);
	old_ms = now_ms = (now_tv.tv_usec / 1000 + now_tv.tv_sec * 1000);
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		gettimeofday(&now_tv, NULL);
		now_ms = (now_tv.tv_usec / 1000 + now_tv.tv_sec * 1000);
		if(now_ms > old_ms) {
			ticks += now_ms - old_ms;
			old_ms = now_ms;
		}
	}
}
