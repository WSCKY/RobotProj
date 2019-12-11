#ifndef __SERVER_H
#define __SERVER_H

#include "gnss.h"

#define SERVER_IP_ADDR                           "36.153.88.121"
#define SERVER_PORT                              "8086"

void rtcm_transfer_task(void const *argument);

#endif /* __SERVER_H */
