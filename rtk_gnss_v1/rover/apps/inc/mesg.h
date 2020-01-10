#ifndef __MESG_H
#define __MESG_H

#include "drivers.h"

#define TYPE_QUAT_Info_Resp                      (0x12)
#define TYPE_RTK_Info_Resp                       (0x61)
#define TYPE_PVTS_Info_Resp                      (0x56)

#define GYR_DATA_MSG                             (0x16)
#define ACC_DATA_MSG                             (0x17)
#define MAG_DATA_MSG                             (0x18)

#define GYR_ORG_DATA_MSG                         (0xC0)
#define ACC_ORG_DATA_MSG                         (0xC1)
#define MAG_ORG_DATA_MSG                         (0xC2)

#define MESG_SUBSCRIBE_REQ_MSG                   (0xEE)

#define FILE_TRANS_REQ_MSG                       (0xB0)
#define FILE_TRANS_DATA_MSG                      (0xB1)
#define FILE_TRANS_ACK_MSG                       (0xB2)

#define MESG_RATE_MAX                            (200) /* 200Hz */

struct MsgInfo {
  uint8_t msg_id;
  uint8_t msg_st;
  uint32_t msg_rt;
  uint8_t msg_pr[4];
};

struct MsgList {
  struct MsgInfo *info;
  struct MsgList *next;
};

void mesg_proc_task(void const *argument);
void mesg_publish_mesg(struct MsgList *list);
void mesg_send_mesg(const void *msg, uint8_t msgid, uint16_t len);

#endif /* __MESG_H */
