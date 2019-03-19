/*
 * acc_calib.h
 *
 *  Created on: Mar 14, 2019
 *      Author: kychu
 */

#ifndef APPS_ACC_CALIB_H_
#define APPS_ACC_CALIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "kyLink.h"
#include "algorithm.h"

int acc_calib_start(void);
void acc_calib_wait_exit(void);

#endif /* APPS_ACC_CALIB_H_ */
