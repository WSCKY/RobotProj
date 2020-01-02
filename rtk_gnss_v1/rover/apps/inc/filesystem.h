/*
 * filesystem.h
 *
 *  Created on: Jan 2, 2020
 *      Author: kychu
 */

#ifndef INC_FILESYSTEM_H_
#define INC_FILESYSTEM_H_

#include "ff_gen_drv.h"

status_t fatfs_mount(void);
status_t fatfs_usage(uint16_t *usage);

#endif /* INC_FILESYSTEM_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
