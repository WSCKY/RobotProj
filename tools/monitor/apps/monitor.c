/*
 * monitor.c
 *
 *  Created on: Feb 18, 2019
 *      Author: kychu
 */

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

#include "uart.h"
#include "terminal.h"

static pthread_t t1, t2;
static int start_flag = 0;
static int exit_flag = 0;

static int init_thread(void);
static void scanThread(void);
static void echoThread(void);

int main(int argc, char *argv[]) {
	int ch;
	char *baud = "115200";
	const char *dev = "/dev/ttyUSB0";

	printf("\e[0;31mMONITOR START\e[0m\n");

	while ((ch = getopt(argc, argv, "d:b:")) != -1) {
		switch (ch) {
			case 'b':
				printf("option -b: %s.\n", optarg);
				baud = optarg;
			break;
			case 'd':
				printf("option -d: %s.\n", optarg);
				dev = optarg;
			break;
			case '?':
				printf("Unknown option: %c\n", (char)optopt);
			break;
		}
	}

	if(uart_open(dev, baud) != EXIT_SUCCESS) {
		printf("\e[0;31mfailed to open uart %s.\n\e[0m", dev);
		return EXIT_FAILURE;
	}

	if(init_thread() != EXIT_SUCCESS) {
		printf("\e[0;31mfailed to create thread.\n\e[0m");
		return EXIT_FAILURE;
	}

	terminal_config();
	start_flag = 1;

	printf("\e[0;33m--- kychu monitor on %s %s ---\n\e[0m", dev, baud);
	printf("\e[0;33m--- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---\n\e[0m");

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	uart_close();

	printf("\n\e[0;31mMONITOR EXIT\e[0m\n");

	terminal_config_restore();

	return EXIT_SUCCESS;
}

static int init_thread(void)
{
	if(pthread_create(&t1, NULL, (void *)scanThread, NULL) != 0) {
		return -1;
	}
	if(pthread_create(&t2, NULL, (void *)echoThread, NULL) != 0) {
		return -1;
	}
	return EXIT_SUCCESS;
}

static void scanThread(void)
{
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		int v = getchar();
		switch(v) {
			case 0x1d:
				exit_flag = 1;
			break;
			default:
			break;
		}
	}
}

static void echoThread(void)
{
	int len = 0;
	char recv[512];
	while(start_flag == 0) {}
	while(exit_flag == 0) {
		len = uart_read(recv, 512);
		if(len > 0) {
			recv[len] = '\0';
			printf("%s", recv);
			fflush(stdout);
		}
	}
}
