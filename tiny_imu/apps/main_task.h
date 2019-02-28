/*
 * main_task.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef APPS_MAIN_TASK_H_
#define APPS_MAIN_TASK_H_

#include "SysConfig.h"

#include "uart2.h"
#include "kyLink.h"
#include "mpu9250.h"

#if FREERTOS_ENABLED
#include "imu_task.h"
#include "com_task.h"
#endif /* FREERTOS_ENABLED */

#endif /* APPS_MAIN_TASK_H_ */
