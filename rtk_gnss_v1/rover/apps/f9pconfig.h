#ifndef __F9PCONFIG_H
#define __F9PCONFIG_H

#include "ubx.h"

#define UBLOX_PROTO_UBX            0x01
#define UBLOX_PROTO_NMEA           0x02
#define UBLOX_PROTO_RTCM2          0x04
#define UBLOX_PROTO_RTCM3          0x20

#define TIME_SYSTEM_UTC            0x00
#define TIME_SYSTEM_GPS            0x01
#define TIME_SYSTEM_GLONASS        0x02
#define TIME_SYSTEM_BEIDOU         0x03
#define TIME_SYSTEM_GALILEO        0x04

#define UBX_NAV                    0x01
#define UBX_NAV_PVT                0x07
#define UBX_NAV_SVIN               0x3B

#define UBX_ACK                    0x05
#define UBX_ACK_NAK                0x00
#define UBX_ACK_ACK                0x01

#define UBX_CFG                    0x06
#define UBX_CFG_PRT                0x00
#define UBX_CFG_MSG                0x01
#define UBX_CFG_RATE               0x08
#define UBX_CFG_TMODE3             0x71
#define UBX_CFG_VALSET             0x8A

__PACK_BEGIN typedef struct {
  uint32_t i_tow;              /* GPS time of week of the navigation epoch. */                     /* ms */
  uint16_t year;               /* Year UTC */                                                      /* y */
  uint8_t  month;              /* Month, range 1..12 (UTC) */                                      /* month */
  uint8_t  day;                /* Day of month, range 1..31 (UTC) */                               /* d */
  uint8_t  hour;               /* Hour of day, range 0..23 (UTC) */                                /* h */
  uint8_t  min;                /* Minute of hour, range 0..59 (UTC) */                             /* min */
  uint8_t  sec;                /* Seconds of minute, range 0..60 (UTC) */                          /* s */
  uint8_t  valid;              /* Validity flags */
  uint32_t t_acc;              /* Time accuracy estimate (UTC) */                                  /* ns */
  int32_t  nano;               /* Fraction of second, range -1e9 .. 1e9 (UTC) */                   /* ns */
  uint8_t  fix_type;           /* GNSSfix Type:
                                    0: no fix
                                    1: dead reckoning only
                                    2: 2D-fix
                                    3: 3D-fix
                                    4: GNSS + dead reckoning combined
                                    5: time only fix */
  uint8_t  flags;              /* Fix status flags */
  uint8_t  flags2;             /* Additional flags */
  uint8_t  num_sv;             /* Number of satellites used in Nav Solution */
  int32_t  lon;                /* Longitude */                                                     /* 1e-7 deg */
  int32_t  lat;                /* Latitude */                                                      /* 1e-7 deg */
  int32_t  height;             /* Height above ellipsoid */                                        /* mm */
  int32_t  h_msl;              /* Height above mean sea level */                                   /* mm */
  uint32_t h_acc;              /* Horizontal accuracy estimate */                                  /* mm */
  uint32_t v_acc;              /* Vertical accuracy estimate */                                    /* mm */
  int32_t  vel_n;              /* NED north velocity */                                            /* mm/s */
  int32_t  vel_e;              /* NED east velocity */                                             /* mm/s */
  int32_t  vel_d;              /* NED down velocity */                                             /* mm/s */
  int32_t  g_speed;            /* Ground Speed (2-D) */                                            /* mm/s */
  int32_t  head_mot;           /* Heading of motion (2-D) */                                       /* 1e-5 deg */
  uint32_t s_acc;              /* Speed accuracy estimate */                                       /* mm/s */
  uint32_t head_acc;           /* Heading accuracy estimate (both motion and vehicle) */           /* 1e-5 deg */
  uint16_t p_dop;              /* Position DOP */                                                  /* 0.01 */
  int32_t  head_veh;           /* Heading of vehicle (2-D) */                                      /* 1e-5 deg */
  int16_t  mag_dec;            /* Magnetic declination */                                          /* 1e-2 deg */
  uint16_t mag_acc;            /* Magnetic declination accuracy */                                 /* 1e-2 deg */
} __PACK_END ubx_npvts_t; /* Navigation Position Velocity Time Solution */

status_t f9p_uart_config(ubx_handle_t *hubx, uint8_t id, uint32_t baudrate, uint16_t inProto, uint16_t outProto);
status_t f9p_get_uart_config(ubx_handle_t *hubx, uint8_t id);

status_t f9p_nav_rate_config(ubx_handle_t *hubx, uint16_t meas_rate, uint16_t nav_rate, uint16_t time_ref);
status_t f9p_msg_rate_config(ubx_handle_t *hubx, uint8_t msg_class, uint8_t msg_id, uint8_t rate_hz);
status_t f9p_surveyin_config(ubx_handle_t *hubx, uint32_t mini_duration, uint32_t limit_in_m);
status_t f9p_poll_msg_config(ubx_handle_t *hubx, uint8_t msg_class, uint8_t msg_id);

status_t f9p_rtcm_msg_output_config(ubx_handle_t *hubx);

#endif /* __F9PCONFIG_H */
