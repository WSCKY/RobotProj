#ifndef __DRIVERS_H
#define __DRIVERS_H

#include "ec20.h"
#include "comif.h"
#include "ubloxa.h"
#include "ubloxb.h"
#include "rtcmif.h"

#include "imuif.h"
#include "icm42605.h"

#if DEBUG_ENABLE
#define dbg_str              comif_tx_string_util
#else
#define dbg_str(...)         ((void)0U)
#endif /* DEBUG_ENABLE */

#endif /* __DRIVERS_H */
