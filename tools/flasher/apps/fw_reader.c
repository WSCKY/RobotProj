/*
 * fw_reader.c
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#include "fw_reader.h"

static int fw_fd = -1;
static struct stat stbuf;
static FW_HEADER fw_header;
static FW_INFO fw_info;
static char header_desc[5] = FW_HEADER_DESC_STRING;

int fw_open(const char *path) {
	fw_fd = open(path, O_RDONLY);
	if(fw_fd == -1) {
		perror(path);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int fw_check(void)
{
	if(read(fw_fd, fw_header.raw, 40) < 40) {
		return -1;
	}
	if(fstat(fw_fd, &stbuf) < 0) {
		return -1;
	}
	for(int i = 0; i < 5; i ++) {
		if(fw_header.header.desc[i] != header_desc[i]) {
			return -1;
		}
	}
	fw_info.size = stbuf.st_size - 40;
	fw_info.crc32 = fw_header.header.crc32;
//	fw_info.method = fw_header.header.method;
	if(fw_header.header.method == 1) {
		fw_info.method = "AES/ECB";
	} else {
		fw_info.method = "PLAIN";
	}
	fw_info.type = fw_header.header.type;
	fw_info.version = fw_header.header.version;
	for(int i = 0; i < 19; i ++) {
		fw_info.time[i] = fw_header.header.time[i];
	}
	fw_info.time[19] = '\0';

	return 0;
}

FW_INFO fw_getInfo(void)
{
	return fw_info;
}

void fw_close(void)
{
	close(fw_fd);
}
