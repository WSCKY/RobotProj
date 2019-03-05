/*
 * fw_reader.h
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#ifndef APPS_FW_READER_H_
#define APPS_FW_READER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

typedef struct {
	char desc[5];
	char time[19];
	unsigned int crc32;
	unsigned short int version;
	unsigned char type;
	unsigned char method;
	unsigned char reserve[8];
} __attribute__((packed)) fw_header_t;

typedef union {
	fw_header_t header;
	unsigned char raw[40];
} __attribute__((packed)) FW_HEADER;

typedef struct {
	char time[20];
	unsigned int crc32;
	unsigned short int version;
	unsigned char type;
	char *method;
	long int size;
} FW_INFO;

#define FW_HEADER_DESC_STRING                  ".kyFW"

int fw_open(const char *path);
int fw_start_read(void);
int fw_read(char *p, size_t l);
int fw_check(void);
void fw_close(void);
FW_INFO fw_getInfo(void);

#endif /* APPS_FW_READER_H_ */
