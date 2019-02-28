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
#include "fw_reader.h"
#include "upgrade.h"

int main(int argc, char *argv[]) {
	int ch;
	char *baud = "115200";
	const char *dev = "/dev/ttyUSB0";
	const char *file = "??.fw";

	terminal_config();

	printf("\e[0;31mUPGRADE START\e[0m\n");
	while ((ch = getopt(argc, argv, "f:d:b:")) != -1) {
		switch (ch) {
			case 'b':
				baud = optarg;
			break;
			case 'd':
				dev = optarg;
			break;
			case 'f':
				file = optarg;
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
	uart_auto_recv_enable();
	uart_set_recv_callback(kyLink_DecodeProcess);

	if(fw_open(file) != EXIT_SUCCESS) {
		printf("\e[0;31mfailed to open fw file %s.\e[0m\n", file);
		terminal_config_restore();
		return EXIT_FAILURE;
	}
	if(fw_check() != 0) {
		printf("\e[0;31mInvalid FW file!\e[0m\n");
		terminal_config_restore();
		return EXIT_FAILURE;
	}

	upgrade_start();
	upgrade_wait_exit();

	fw_close();
	uart_close();

	printf("\n\e[0;31mUPGRADE DONE\e[0m\n");

	terminal_config_restore();

	return EXIT_SUCCESS;
}
