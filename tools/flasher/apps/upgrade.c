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

static long int task_ticks = 0;

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

static int rec_time_out = 0;

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
	printf("Size: %ldB, V%d.%d.%d, Type:%d, Text:%s @ %s\n", fw_head.size, major, minor, fixnum, fw_head.type, fw_head.method, fw_head.time);
	while(start_flag == 0) {}
	printf("Start bootloader ...\n");

	txPacket.Packet.msg_id = TYPE_UPGRADE_REQUEST;
	txPacket.Packet.length = sizeof(FWInfoDef);
	txPacket.Packet.PacketData.FileInfo.FW_Type = fw_head.type;
	txPacket.Packet.PacketData.FileInfo.Enc_Type = 0;
	txPacket.Packet.PacketData.FileInfo.FW_Version = fw_head.version;
	txPacket.Packet.PacketData.FileInfo.FileCRC = fw_head.crc32;
	txPacket.Packet.PacketData.FileInfo.FileSize = fw_head.size;
	txPacket.Packet.PacketData.FileInfo.PacketNum = (fw_head.size / FILE_DATA_CACHE) + (((fw_head.size % FILE_DATA_CACHE) > 0) ? 1 : 0);

	printf("EARSING "); fflush(stdout);
	time_a = task_ticks;
	while(exit_flag == 0) {
		while(task_ticks - time_a < 5000 && rec_time_out == 0) { // wait 5s
			printf("."); fflush(stdout);
			usleep(200000);
			SendTxPacket(&txPacket);
		}
		exit_flag = 1;
	}
}

static CommPackageDef *pRx;
static void upgrade_rx_task(void)
{
	long int trec = 0;
	pRx = GetRxPacket();
	while(start_flag == 0) {}
	trec = task_ticks;
	while(exit_flag == 0) {
		if(GotNewData()) {
			trec = task_ticks;
			printf("GOT DATA(0x%x:0x%x)\n", pRx->Packet.dev_id, pRx->Packet.msg_id);
		}
		if(rec_time_out == 0) {
			if(task_ticks - trec > 3000) {
				// time out.
				rec_time_out = 1;
				printf("\n\e[0;31mTIME OUT!!!\e[0m\n");
			}
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
			task_ticks += now_ms - old_ms;
			old_ms = now_ms;
		}
	}
}
