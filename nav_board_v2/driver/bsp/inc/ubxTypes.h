#ifndef __UBX_TYPES_H
#define __UBX_TYPES_H

#include "SysConfig.h"

__PACK_BEGIN typedef struct {

	uint8_t SYNCHAR1, SYNCHAR2;
	uint8_t CLASS, ID;
	uint8_t LEN_L, LEN_H;
	uint8_t* pPayload;
	uint8_t CK_A, CK_B;
}__PACK_END UBXStrcutureDef;

__PACK_BEGIN typedef struct {
	unsigned char  numSV; /* Number of satellites used in Nav Solution */
	int lon;               /* Longitude */
	int lat;               /* Latitude */
	int height;            /* Height above ellipsoid */
	int hMSL;              /* Height above mean sea level */
	unsigned short year;   /* Year (UTC) */
	unsigned char month;   /* Month, range 1..12 (UTC) */
	unsigned char day;     /* Day of month, range 1..31 (UTC) */
	unsigned char hour;    /* Hour of day, range 0..23 (UTC) */
	unsigned char min;     /* Minute of hour, range 0..59 (UTC) */
	unsigned char sec;     /* Seconds of minute, range 0..60 (UTC) */
	/* GNSS fix Type:
      0: no fix
      1: dead reckoning only
      2: 2D-fix
      3: 3D-fix
      4: GNSS + dead reckoning combined
      5: time only fix
  */
  unsigned char fixType;
	unsigned int hAcc;     /* Horizontal accuracy estimate */
	unsigned int vAcc;     /* Vertical accuracy estimate */
	int velN;              /* NED north velocity */
	int velE;              /* NED east velocity */
	int velD;              /* NED down velocity */
  int headVeh;           /* Heading of vehicle (2-D) */
	unsigned int sAcc;     /* Speed accuracy estimate */
}__PACK_END GPS_DATA_RAW;

#endif /* __UBX_TYPES_H */
