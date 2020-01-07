/*
 * filetransfer.h
 *
 *  Created on: Jan 7, 2020
 *      Author: kychu
 */

#ifndef INC_FILETRANSFER_H_
#define INC_FILETRANSFER_H_

#include "filetransfer_type.h"
#include "ff_gen_drv.h"
#include "mesg.h"
#include "kyLink.h"

void transfile_task(void const *argument);
void filetransfer_cmd_process(kyLinkBlockDef *pRx);

#endif /* INC_FILETRANSFER_H_ */
