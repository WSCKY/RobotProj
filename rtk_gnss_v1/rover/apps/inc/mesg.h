#ifndef __MESG_H
#define __MESG_H

#include "drivers.h"

#define TYPE_QUAT_Info_Resp                      (0x12)
#define TYPE_RTK_Info_Resp                       (0x61)
#define TYPE_PVTS_Info_Resp                      (0x56)

#define FILE_TRANS_REQ_MSG                       (0xB0)
#define FILE_TRANS_DATA_MSG                      (0xB1)
#define FILE_TRANS_ACK_MSG                       (0xB2)

void mesg_proc_task(void const *argument);
void mesg_send_mesg(const void *msg, uint8_t msgid, uint16_t len);

#endif /* __MESG_H */
