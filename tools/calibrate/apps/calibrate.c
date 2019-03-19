/*
 * boot.c
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
#include "acc_calib.h"

KYLINK_CORE_HANDLE kyLinkHandle;

static void kyLinkWrite(uint8_t *p, uint32_t l);
static void kyLinkDecodeHandler(unsigned char Data);

int main(int argc, char *argv[]) {
	int ch;
	char *baud = "115200";
	const char *dev = "/dev/ttyUSB0";
	int calib_mode = 0; /* 0 for accelerometer calibration, 1 for magnetometer calibration */

	terminal_config();

	printf("\e[0;31mCALIBRATION START\e[0m\n");
	while ((ch = getopt(argc, argv, "d:b:m:")) != -1) {
		switch (ch) {
			case 'b':
				baud = optarg;
			break;
			case 'd':
				dev = optarg;
			break;
			case 'm':
				calib_mode = atoi(optarg);
			break;
			case '?':
				printf("Unknown option: %c\n", (char)optopt);
			break;
		}
	}

	switch(calib_mode) {
		case 0:
			printf("calibrate accelerometer.\n");
		break;
		case 1:
			printf("calibrate magnetometer.\n");
		break;
		default:
			calib_mode = 0;
			printf("Unknown calibration mode, default calibrate accelerometer.\n");
		break;
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

	if(calib_mode == 0) {
		acc_calib_start();
		acc_calib_wait_exit();
	} else if(calib_mode == 1) {
		// ...
	}

	uart_close();

	printf("\n\e[0;31mCALIBRATION DONE\e[0m\n");

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
