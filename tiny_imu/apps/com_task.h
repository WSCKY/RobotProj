#ifndef __COM_TASK_H
#define __COM_TASK_H

#if FREERTOS_ENABLED

#include "kyLink.h"

//#include "ComPort.h"
//
//#include "usbd_cdc_core.h"
//#include "usbd_usr.h"
//#include "usb_conf.h"
//#include "usbd_desc.h"
//#include "usbd_cdc_vcp.h"

typedef struct {
  uint8_t type;
  uint8_t len;
  void *pointer;
} __packed COM_MSG_DEF;

void COM_Thread(void const *argument);
QueueHandle_t* get_com_msg_send_queue(void);

#endif /* FREERTOS_ENABLED */

#endif /* __COM_TASK_H */