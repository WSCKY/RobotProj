/*
 * upgrade.h
 *
 *  Created on: Feb 26, 2019
 *      Author: kychu
 */

#ifndef APPS_UPGRADE_H_
#define APPS_UPGRADE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "kyLink.h"
#include "fw_reader.h"

int upgrade_start(void);
void upgrade_wait_exit(void);

#endif /* APPS_UPGRADE_H_ */
