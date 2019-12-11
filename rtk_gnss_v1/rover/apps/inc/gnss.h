#ifndef __GNSS_H
#define __GNSS_H

#include "drivers.h"
#include "f9pconfig.h"

void gnss_navg_task(void const *argument);

bool_t check_rtk_rover_ready(void);
ubx_npvts_t *get_npvts_a(void);
ubx_npvts_t *get_npvts_b(void);

#endif /* __GNSS_H */
