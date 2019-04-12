/*
 * com_task.c
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#include "com_task.h"

static pthread_t tx_thread;
static pthread_t rx_thread;
static pthread_t tim_thread;
static pthread_t exit_thread;
static int start_flag = 0;
static int exit_flag = 0;

static long int task_ticks = 0;

static void timer_task(void);
static void com_tx_task(void);
static void com_rx_task(void);
static void com_exit_task(void);

int com_start(void)
{
	if(pthread_create(&tim_thread, NULL, (void *)timer_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&tx_thread, NULL, (void *)com_tx_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&rx_thread, NULL, (void *)com_rx_task, NULL) != 0)
		return EXIT_FAILURE;
	if(pthread_create(&exit_thread, NULL, (void *)com_exit_task, NULL) != 0)
		return EXIT_FAILURE;
	start_flag = 1;
	return EXIT_SUCCESS;
}

void com_wait_exit(void)
{
	pthread_join(tx_thread, NULL);
	pthread_join(rx_thread, NULL);
	pthread_join(tim_thread, NULL);
	pthread_join(exit_thread, NULL);
}

extern KYLINK_CORE_HANDLE kyLinkHandle;
static kyLinkPackageDef txPacket = {0};

static int tx_enable = 0;
static uint8_t tx_cnt = 0;
static int tx_delay = 200; /* default tx delay 200ms */
static int tx_time_stamp = 0;
static void com_tx_task(void)
{
	kyLinkInitPackage(&txPacket);
	while(start_flag == 0) {}
	tx_delay = 200; // delay 200ms
	tx_time_stamp = task_ticks;
	printf("sending heartbeat per 200ms ...\n");
	while(exit_flag == 0) {
		if(task_ticks - tx_time_stamp > tx_delay) {
			tx_enable = 1;
		}
		if(tx_enable != 0) {
			tx_enable = 0;
			txPacket.FormatData.PacketData.TypeData.Heartbeat = tx_cnt ++;
			SendTxPacket(&kyLinkHandle, &txPacket);
			tx_time_stamp = task_ticks;
		}
	}
}

static int rx_cnt = 0;
static kyLinkPackageDef *pRx;
static void com_rx_task(void)
{
	static long int timestamp = 0;
	pRx = GetRxPackage(&kyLinkHandle);
	while(start_flag == 0) {}
	timestamp = task_ticks;
	while(exit_flag == 0) {
		if(kyLinkCheckUpdate(&kyLinkHandle) == kyTRUE) {
			rx_cnt ++;
			printf("rx msg_id: %d, cnt: %d.\n", pRx->FormatData.msg_id, rx_cnt);
		} else {
			if(task_ticks - timestamp > 2000) {
				printf("\e[0;31mCONNECTION LOST.\e[0m\n");
				timestamp = task_ticks;
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

static void com_exit_task(void)
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
