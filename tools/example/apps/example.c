/*
 * example.c
 *
 *  Created on: Feb 25, 2019
 *      Author: kychu
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "kyLink.h"

#include "uart.h"
#include "terminal.h"
#include "com_task.h"

KYLINK_CORE_HANDLE kyLinkHandle;

static void kyLinkWrite(uint8_t *p, uint32_t l);
static void kyLinkDecodeHandler(unsigned char Data);

int main(int argc, char *argv[]) {
	int ch;
	char *baud = "115200";
	const char *dev = "/dev/ttyUSB0";

	terminal_config();

	printf("\e[0;31mkylink TEST START\e[0m\n");
	while ((ch = getopt(argc, argv, "d:b:")) != -1) {
		switch (ch) {
			case 'b':
				baud = optarg;
			break;
			case 'd':
				dev = optarg;
			break;
			case '?':
				printf("Unknown option: %c\n", (char)optopt);
			break;
		}
	}

	if(uart_open(dev, baud) != EXIT_SUCCESS) {
		printf("\e[0;31mfailed to open uart %s.\e[0m\n", dev);
		terminal_config_restore();
		return EXIT_FAILURE;
	}

	kyLinkInit(&kyLinkHandle, kyLinkWrite);
	kyLinkTxEnable(&kyLinkHandle);

	uart_set_recv_callback(kyLinkDecodeHandler);
	uart_auto_recv_enable();

	com_start();
	com_wait_exit();

	uart_close();

	printf("\n\e[0;31mTEST DONE\e[0m\n");

	terminal_config_restore();

	return EXIT_SUCCESS;
}

static void kyLinkWrite(uint8_t *p, uint32_t l)
{
	uart_write((char *)p, (size_t)l);
}

static void kyLinkDecodeHandler(unsigned char Data)
{
	kylink_decode(&kyLinkHandle, Data);
}
