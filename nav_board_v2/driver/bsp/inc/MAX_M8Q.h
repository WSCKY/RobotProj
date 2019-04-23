#ifndef __MAX_M8Q_H
#define __MAX_M8Q_H

#include "ubxUART.h"
#include "ubxTypes.h"

#if FREERTOS_ENABLED
#define ubx_delay                      osDelay
#else
#include "TimerCounter.h"
#define ubx_delay                      _delay_ms
#endif /* FREERTOS_ENABLED */

#define ubx_send_bytes                 ubxPortSendBytes

typedef enum {
  UBX_OK = 0,
  UBX_FAILED = 1,
  UBX_UNCONFIG = 2,
  UBX_CONFIGED = 3
} UBX_ERROR;

typedef enum {
	portable =  0,
	stationary = 2,
	pedestrian = 3,
	automotive = 4,
	sea        = 5,
	lessThan1G = 6,
	lessThan2G = 7,
	lessThan4G = 8
} DYNC_PLAT_MODEL;

#define UBX_SYN_STX1                   (0xB5)
#define UBX_SYN_STX2                   (0x62)

typedef enum {
  UBX_DECODE_UNSYNCED   = 0,
  UBX_DECODE_GOT_SYN1   = 1,
  UBX_DECODE_GOT_SYN2   = 2,
  UBX_DECODE_GOT_CLASS  = 3,
  UBX_DECODE_GOT_ID     = 4,
  UBX_DECODE_GOT_LEN_L  = 5,
  UBX_DECODE_GOT_LEN_H  = 6,
  UBX_DECODE_GOT_DATA   = 7,
  UBX_DECODE_GOT_CK_A   = 8,
} UBX_DECODE_STATE;

#define UBX_CLASS_NAV                  (0x01)
#define UBX_ID_NAV_PVT                 (0x07)
#define UBX_ID_NAV_SAT                 (0x35)

#define UBX_CLASS_CFG                  (0x06)
#define UBX_ID_CFG_PRT                 (0x00)
#define UBX_ID_CFG_MSG                 (0x01)
#define UBX_ID_CFG_RATE                (0x08)
#define UBX_ID_CFG_NAV5                (0x24)

#define UBX_CLASS_ACK                  (0x05)
#define UBX_ID_ACK_ACK                 (0x01)
#define UBX_ID_ACK_NAK                 (0x00)
  
UBX_ERROR ublox_m8q_init(void);

#endif /* __MAX_M8Q_H */
