#ifndef __MESG_H
#define __MESG_H

#include "drivers.h"

typedef enum {
  msg_read = 0,
  msg_write = 1
} msg_wr_state;

__PACK_BEGIN typedef struct {
  uint32_t type;
  uint32_t len;
  void *state;
  void *pointer;
} __PACK_END COM_MSG_DEF;

#define TYPE_QUAT_Info_Resp                      (0x12)
#define TYPE_RTK_Info_Resp                       (0x61)
#define TYPE_PVTS_Info_Resp                      (0x56)

void mesg_send_task(void const *argument);
void mesg_send_mesg(void *msg);

#endif /* __MESG_H */
