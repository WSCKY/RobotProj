/*
 * upgrade.c
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#include "upgrade.h"

#define COMBINE_4BYTES(a, b, c, d)     ((((uint32_t)(d) << 24) & 0xFF000000) | \
		(((uint32_t)(c) << 16) & 0x00FF0000) | \
		(((uint32_t)(b) << 8) & 0x0000FF00) | ((a) & 0x000000FF))

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

FW_INFO fw_head;
static CommPackageDef txPacket = {0};

static UpgradeStep _step = Upgrade_SendReq;
static int FlashPages = 0, FlashErased = 0;
static int ReqPackageID = -1, TxPackageID = -1;
static int PackageNbr = 0;

static int tx_stop = 0;
static int tx_enable = 0;
static int tx_delay = 200; /* default tx delay 200ms */
static int tx_time_stamp = 0;
static void upgrade_tx_task(void)
{
	fw_head = fw_getInfo();
	InitCommPackage(&txPacket);
	int major = (fw_head.version & 0x00F0) >> 4;
	int minor = fw_head.version & 0x000F;
	int fixnum = (fw_head.version >> 8) & 0x00FF;
	printf("Size: %ldB, V%d.%d.%d, Type:%d, Text:%s @ %s\n", fw_head.size, major, minor, fixnum, fw_head.type, fw_head.method, fw_head.time);

	txPacket.Packet.msg_id = TYPE_UPGRADE_REQUEST;
	txPacket.Packet.length = sizeof(FWInfoDef);
	txPacket.Packet.PacketData.FileInfo.FW_Type = fw_head.type;
	txPacket.Packet.PacketData.FileInfo.Enc_Type = 0;
	txPacket.Packet.PacketData.FileInfo.FW_Version = fw_head.version;
	txPacket.Packet.PacketData.FileInfo.FileCRC = fw_head.crc32;
	txPacket.Packet.PacketData.FileInfo.FileSize = fw_head.size;
	PackageNbr = (fw_head.size / FILE_DATA_CACHE) + (((fw_head.size % FILE_DATA_CACHE) > 0) ? 1 : 0);
	txPacket.Packet.PacketData.FileInfo.PacketNum = PackageNbr;

	while(start_flag == 0) {}
	printf("Start bootloader ...\n");

	tx_delay = 200; // delay 200ms
	tx_time_stamp = task_ticks;
	while(exit_flag == 0) {
		if(task_ticks - tx_time_stamp > tx_delay) {
			tx_enable = 1;
		}

		if(tx_stop == 0) {
			if(tx_enable != 0) {
				tx_enable = 0;
				SendTxPacket(&txPacket);
				tx_time_stamp = task_ticks;
			}
		} else {
			tx_enable = 0;
			tx_time_stamp = task_ticks;
		}
		usleep(100);
	}
}

static CommPackageDef *pRx;
static int rx_time_stamp = 0;

static int last_wait = -1;

static void upgrade_rx_task(void)
{
	long int time_start = 0;
	pRx = GetRxPacket();
	_step = Upgrade_SendReq;
	rx_time_stamp = task_ticks;
	while(exit_flag == 0) {
		if(GotNewData()) {
			rx_time_stamp = task_ticks;
			if(_step == Upgrade_SendReq) {
				if(pRx->Packet.msg_id == TYPE_UPGRADE_DEV_ACK && pRx->Packet.PacketData.DevRespInfo.Dev_State == InErasing) {
					tx_stop = 1; // pause.
					FlashPages = pRx->Packet.PacketData.DevRespInfo.reserve[0];
					FlashErased = pRx->Packet.PacketData.DevRespInfo.reserve[1];
					printf("WAIT EARSE ... (%02d/%02d)  \r", FlashErased, FlashPages); fflush(stdout);
				}
			}
			if(pRx->Packet.msg_id == TYPE_UPGRADE_DEV_ACK && pRx->Packet.PacketData.DevRespInfo.Dev_State == Upgrading) {
				if(_step != Upgrade_SendDat) {
					_step = Upgrade_SendDat;
					printf("WAIT EARSE ... (%02d/%02d)\n", FlashPages, FlashPages); fflush(stdout);
					txPacket.Packet.msg_id = TYPE_UPGRADE_DATA;
					txPacket.Packet.length = sizeof(UpgradeDataDef);
					fw_start_read(); // set read pointer.
					tx_delay = 10; // delay 10ms.
					time_start = task_ticks;
				}
				// get request package id.
				ReqPackageID = COMBINE_4BYTES(pRx->Packet.PacketData.DevRespInfo.reserve[0], \
											  pRx->Packet.PacketData.DevRespInfo.reserve[1], \
											  pRx->Packet.PacketData.DevRespInfo.reserve[2], \
											  pRx->Packet.PacketData.DevRespInfo.reserve[3]);
				if(ReqPackageID > TxPackageID) {
					TxPackageID = ReqPackageID;
					txPacket.Packet.PacketData.PacketInfo.PacketID = ReqPackageID;
					if(ReqPackageID < PackageNbr) {
						int len = fw_read((char *)(txPacket.Packet.PacketData.PacketInfo.TextData), FILE_DATA_CACHE);
						if(len > 0) {
							txPacket.Packet.PacketData.PacketInfo.PacketLen = len;
							tx_stop = 0; // tx continuously
							tx_enable = 1; // tx immediately
							printf("PROGRAM ... (%d/%d)     \r", ReqPackageID + 1, PackageNbr); fflush(stdout);
						} else {
							printf("\n\e[0;31mFW READ ERROR!!!\e[0m\n");
						}
						if(ReqPackageID == PackageNbr - 1) { // the last one package, maybe lost.
							last_wait = task_ticks;
						}
					} else if(ReqPackageID == PackageNbr) {
						printf("\nPROGRAM TIME: %ldms, RATE: %2.2fKB/s\n", task_ticks - time_start, (float)fw_head.size / (task_ticks - time_start));
						exit_flag = 1;
					}
				}
			}
		}
		usleep(100);
		if(task_ticks - rx_time_stamp > 10000) {	// time out (10s).
			exit_flag = 1;
			printf("\n\e[0;31mUPGRADE TIME OUT!!!\e[0m\n");
		}
		if(last_wait > 0) {
			if(task_ticks - last_wait > 2000) {
				printf("\n\e[0;33mLOST THE LAST PACKAGE ...\e[0m\n");
				exit_flag = 1;
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
		usleep(500);
	}
}
