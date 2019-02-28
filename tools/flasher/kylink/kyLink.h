#ifndef __KYLINK_H
#define __KYLINK_H

#include "SysConfig.h"
#include "LinkIf_conf.h"
#include "LinkVersion.h"

#include "ComTypes.h"

#ifndef MAIN_DATA_CACHE
  #define MAIN_DATA_CACHE                        (32)
#endif /* MAIN_DATA_CACHE */

#define PACKET_CACHE                             (MAIN_DATA_CACHE + 8)

#define kySTX1                                   (0x55)
#define kySTX2                                   (0xAA)

typedef enum {
  DECODE_STATE_UNSYNCED  = 0,
  DECODE_STATE_GOT_STX1  = 1,
  DECODE_STATE_GOT_STX2  = 2,
  DECODE_STATE_GOT_DEVID = 3,
  DECODE_STATE_GOT_MSGID = 4,
  DECODE_STATE_GOT_LEN_L = 5,
  DECODE_STATE_GOT_LEN_H = 6,
  DECODE_STATE_GOT_DATA  = 7,
  DECODE_STATE_GOT_CRC_L = 8,
} __packed DECODE_STATE;

typedef enum {
  /* Communication Heartbeat */
  TYPE_LINK_HEARTBEAT = 0x01,
  /* Protocol Information */
  TYPE_LINKRE_VER_Req = 0x02,
  TYPE_LINKER_VER_Resp = 0x03,
  TYPE_LINKER_NAME_Req = 0x04,
#if defined(COM_USER_TYPE)
  COM_USER_TYPE
#endif
} __packed PACKET_TYPE;

typedef struct {
	uint8_t _Cnt;
} __packed HeartBeatDef;

typedef struct {
	uint16_t v;
} __packed VersionResponseDef;

typedef union {
	uint8_t pData[MAIN_DATA_CACHE];
	HeartBeatDef Heartbeat;
	VersionResponseDef Version;
#if defined(COM_USER_TYPE_DATA)
    COM_USER_TYPE_DATA
#endif
} __packed PacketDataUnion;

typedef struct {
  uint8_t stx1;
  uint8_t stx2;
  uint8_t dev_id;
  uint8_t msg_id;
  uint16_t length;
  PacketDataUnion PacketData;
  uint16_t crc16;
} __packed PackageStructDef;

typedef union {
	PackageStructDef Packet;
	uint8_t RawData[PACKET_CACHE];
} __packed CommPackageDef;

uint8_t GotNewData(void);
CommPackageDef* GetRxPacket(void);
void InitCommPackage(CommPackageDef* pPacket);

void SendTxPacket(CommPackageDef* pPacket);
void kyLink_DecodeProcess(uint8_t data);

#ifndef HARD_DEV_ID
  #define HARD_DEV_ID                            (0)
#endif /* HARD_DEV_ID */

#ifndef COM_IF_TX_CHECK
  #define COM_IF_TX_CHECK()                      (0)
#endif /* COM_IF_TX_CHECK */

#ifndef COM_IF_TX_BYTES
  #define COM_IF_TX_BYTES(...)                   ((void)0)
#endif /* COM_IF_TX_BYTES */

#endif /* __KYLINK_H */
