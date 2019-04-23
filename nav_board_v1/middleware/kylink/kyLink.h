#ifndef __KYLINK_H
#define __KYLINK_H

#include "SysConfig.h"
#include "LinkIf_conf.h"
#include "LinkVersion.h"

#include "ComTypes.h"

#ifndef MAIN_DATA_CACHE
  #define MAIN_DATA_CACHE                        (32) /* 16 * n */
#endif /* MAIN_DATA_CACHE */

#define PACKET_CACHE                             (MAIN_DATA_CACHE + 8)

#if FREERTOS_ENABLED
  #ifndef MSG_QUEUE_DEPTH
    #define MSG_QUEUE_DEPTH                      (3)
  #endif /* MSG_QUEUE_DEPTH */
#endif /* FREERTOS_ENABLED */

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
} DECODE_STATE;

__PACK_BEGIN typedef enum {
	/* Communication Heartbeat */
	TYPE_LINK_HEARTBEAT = 0x01,
	/* Protocol Information */
	TYPE_LINKRE_VER_Req = 0x02,
	TYPE_LINKER_VER_Resp = 0x03,
  TYPE_LINKER_NAME_Req = 0x04,
#if defined(COM_USER_TYPE)
  COM_USER_TYPE
#endif
} __PACK_END PACKET_TYPE;

__PACK_BEGIN typedef struct {
	uint8_t _Cnt;
} HeartBeatDef;

__PACK_BEGIN typedef struct {
	uint16_t v;
} __PACK_END VersionResponseDef;

__PACK_BEGIN typedef union {
	uint8_t pData[MAIN_DATA_CACHE];
	HeartBeatDef Heartbeat;
	VersionResponseDef Version;
#if defined(COM_USER_TYPE_DATA)
  COM_USER_TYPE_DATA
#endif
} __PACK_END PacketDataUnion;

__PACK_BEGIN typedef struct {
	uint8_t stx1;
	uint8_t stx2;
  uint8_t dev_id;
  uint8_t msg_id;
	uint16_t length;
	PacketDataUnion PacketData;
	uint16_t crc16;
} __PACK_END PackageStructDef;

__PACK_BEGIN typedef union {
	PackageStructDef Packet;
	uint8_t RawData[PACKET_CACHE];
} __PACK_END CommPackageDef;

#if FREERTOS_ENABLED
extern QueueHandle_t lnk_recv_q;
#else
uint8_t GotNewData(void);
CommPackageDef* GetRxPacket(void);
#endif /* FREERTOS_ENABLED */

void kyLink_Init(void);
void SendTxPacket(CommPackageDef* pPacket);
void kyLink_DecodeProcess(uint8_t data);

#ifndef COM_IF_TX_CHECK
  #define COM_IF_TX_CHECK()                      (0)
#endif /* COM_IF_TX_CHECK */

#ifndef COM_IF_TX_BYTES
  #define COM_IF_TX_BYTES(...)                   ((void)0)
#endif /* COM_IF_TX_BYTES */

#endif /* __KYLINK_H */
