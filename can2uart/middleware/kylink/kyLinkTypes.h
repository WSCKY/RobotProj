#ifndef __KYLINKTYPES_H
#define __KYLINKTYPES_H

#include "kyLinkMacros.h"
/* user configure file */
#include "LinkIf_conf.h"

/*
 * Default Definitions
 */
#ifndef HARD_DEV_ID
  #define HARD_DEV_ID                            (0)
#endif /* HARD_DEV_ID */

/* Frame Header */
#define kySTX1                                   (0x55)
#define kySTX2                                   (0xAA)

typedef void (*PortRecvByteCallback)(uint8_t Data);
typedef void (*kyLinkPortTxBytesFunc)(uint8_t *, uint32_t);

#ifndef NULL
#define NULL                                     (0)
#endif /* NULL */

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
#if defined(KYLINK_USER_TYPE)
  KYLINK_USER_TYPE
#endif
} __PACK_END PACKET_TYPE;

__PACK_BEGIN typedef union {
	uint8_t Heartbeat;
	uint8_t VersionReq;
	uint16_t VersionAck;
#if defined(KYLINK_USER_TYPE_DATA)
	KYLINK_USER_TYPE_DATA
#endif
} __PACK_END PackTypeDataDef;

#define MAIN_DATA_CACHE                          sizeof(PackTypeDataDef)

__PACK_BEGIN typedef union {
	uint8_t RawData[MAIN_DATA_CACHE];
	PackTypeDataDef TypeData;
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

#define PACKET_CACHE                             sizeof(PackageStructDef)

__PACK_BEGIN typedef union {
	PackageStructDef FormatData;
	uint8_t RawData[PACKET_CACHE];
} __PACK_END kyLinkPackageDef;

__PACK_BEGIN typedef struct {
	/* decoder required. */
	BooleanState update_flag;
	uint32_t rx_counter;
	DECODE_STATE _decode_state;
	kyLinkPackageDef _rx_packet;
	kyLinkPackageDef _rx_packet_copy;
} __PACK_END kyLinkDecoderDef;

__PACK_BEGIN typedef struct {
	BooleanState port_enable;
	kyLinkDecoderDef decoder;
	kyLinkPortTxBytesFunc port_tx;
} __PACK_END KYLINK_CORE_HANDLE;

#endif /* __KYLINKTYPES_H */
