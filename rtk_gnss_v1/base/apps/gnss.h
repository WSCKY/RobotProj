#ifndef __GNSS_H
#define __GNSS_H

#include "drivers.h"

#include "server.h"

void gnss_navg_task(void const *argument);

bool_t check_rtk_base_ready(void);

#endif /* __GNSS_H */
