/*
 * printPacket.c
 *
 *  Created on: Mar 4, 2019
 *      Author: kychu
 */

#include <stdio.h>

void debug_char(char c) {
	printf("0x%02x  ", c);
}

int debug_write(char *p, size_t l)
{
	while(l --) {
		debug_char(*p);
		p ++;
	}
	fflush(stdout);
}
