/*
 * com_task.h
 *
 *  Created on: Mar 11, 2019
 *      Author: kychu
 */

#ifndef APPS_COM_TASK_H_
#define APPS_COM_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#include "kyLink.h"

int com_start(void);
void com_wait_exit(void);

#endif /* APPS_COM_TASK_H_ */
