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

#define FLASH_PAGE_SIZE                ((uint32_t)0x00000400)   /* FLASH Page Size */
#define FLASH_END_ADDRESS              ((uint32_t)0x08008000)

#define APPLICATION_ADDRESS            ((uint32_t)SYS_TEXT_ORIGIN)   /* Start @ of user Flash area */

#define PACKAGE_NUM_PER_CACHE          (BOOT_CACHE_SIZE / FILE_DATA_CACHE)

typedef  void (*pFunction)(void);

#endif /* APPS_MAIN_TASK_H_ */
