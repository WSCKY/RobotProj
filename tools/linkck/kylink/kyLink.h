/**
 * @file    kyLink.h
 * @author  kyChu
 * @date    2017/08/20
 * @version V0.8.0
 * @brief   header file for kyLink.c
 */

/* Define to prevent recursive inclusion */
#ifndef __KYLINK_H
#define __KYLINK_H

/*
 * Includes
 */
#include "ComTypes.h"
#include "LinkVersion.h"
/* user configure file */
#include "LinkIf_conf.h"

/*
 * Default Definitions
 */
#ifndef HARD_DEV_ID
  #define HARD_DEV_ID                            (0)
#endif /* HARD_DEV_ID */

#ifndef COM_IF_TX_CHECK
  #define COM_IF_TX_CHECK()                      (0)
#endif /* COM_IF_TX_CHECK */

#ifndef COM_IF_TX_BYTES
  #define COM_IF_TX_BYTES(...)                   ((void)0)
#endif /* COM_IF_TX_BYTES */

#ifndef MAIN_DATA_CACHE
  #define MAIN_DATA_CACHE                        (32)
#endif /* MAIN_DATA_CACHE */

/* CACHE SIZE */
#define PACKET_CACHE                             (MAIN_DATA_CACHE + 8)

/* Frame Header */
#define kySTX1                                   (0x55)
#define kySTX2                                   (0xAA)

/* Frame Structure definitions */
__PACK_BEGIN typedef enum {
  DECODE_STATE_UNSYNCED  = 0,
  DECODE_STATE_GOT_STX1  = 1,
  DECODE_STATE_GOT_STX2  = 2,
  DECODE_STATE_GOT_DEVID = 3,
  DECODE_STATE_GOT_MSGID = 4,
  DECODE_STATE_GOT_LEN_L = 5,
  DECODE_STATE_GOT_LEN_H = 6,
  DECODE_STATE_GOT_DATA  = 7,
  DECODE_STATE_GOT_CRC_L = 8,
} __PACK_END DECODE_STATE;

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

__PACK_BEGIN typedef union {
	uint8_t pData[MAIN_DATA_CACHE];
	uint8_t Heartbeat;
	uint8_t VersionReq;
	uint16_t VersionAck;
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

/*
 * Export function prototypes
 */
uint8_t GotNewData(void);
CommPackageDef* GetRxPacket(void);
void InitCommPackage(CommPackageDef* pPacket);

void SendTxPacket(CommPackageDef* pPacket);
void kyLink_DecodeProcess(uint8_t data);

#endif /* __KYLINK_H */

/**
 * @ End of file.
 */
