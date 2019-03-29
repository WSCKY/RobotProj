/**
 * @file    kyLink.c
 * @author  kyChu
 * @date    2017/08/20
 * @version V1.2.0
 * @brief   kylink protocol in ANSI C
 */
#include "kyLink.h"

/* function prototypes */
static uint16_t do_crc_table(uint8_t *ptr, uint32_t len);

void kyLinkInit(KYLINK_CORE_HANDLE *pHandle, kyLinkPortTxBytesFunc pTx)
{
	pHandle->port_tx = pTx;
	pHandle->port_enable = kyFALSE;
	pHandle->decoder.rx_counter = 0;
	pHandle->decoder.update_flag = 0;
	pHandle->decoder._decode_state = DECODE_STATE_UNSYNCED;
}

BooleanState kyLinkCheckUpdate(KYLINK_CORE_HANDLE *pHandle)
{
	if(pHandle->decoder.update_flag == kyTRUE) {
		pHandle->decoder.update_flag = kyFALSE;
		return kyTRUE;
	}
	return kyFALSE;
}

void kyLinkInitPackage(kyLinkPackageDef *pPack)
{
  pPack->FormatData.stx1 = kySTX1;
  pPack->FormatData.stx2 = kySTX2;
  pPack->FormatData.dev_id = HARD_DEV_ID;
  pPack->FormatData.msg_id = TYPE_LINK_HEARTBEAT;
  pPack->FormatData.length = 1;
  pPack->FormatData.PacketData.TypeData.Heartbeat = 0;
  pPack->FormatData.crc16 = 0;
}

void kyLinkTxEnable(KYLINK_CORE_HANDLE *pHandle)
{
	pHandle->port_enable = kyTRUE;
}

void kyLinkTxDisable(KYLINK_CORE_HANDLE *pHandle)
{
	pHandle->port_enable = kyFALSE;
}

kyLinkPackageDef* GetRxPackage(KYLINK_CORE_HANDLE *pHandle)
{
	return &pHandle->decoder._rx_packet_copy;
}

static uint16_t do_crc_table(uint8_t *ptr, uint32_t len)
{
  unsigned short int crc;
  uint8_t H8;

  crc = 0;
  while(len -- != 0) {
    H8 = (unsigned short)crc >> 8;
    crc <<= 8;
    crc ^= crcTab16[H8 ^ *ptr];
    ptr ++;
  }
  return(crc);
}

#pragma GCC push_options
#pragma GCC optimize ("O0")
void SendTxPacket(KYLINK_CORE_HANDLE *pHandle, kyLinkPackageDef* pPacket)
{
  pPacket->FormatData.crc16 = do_crc_table(&(pPacket->RawData[2]), pPacket->FormatData.length + 4);
  /* NOTE: DO NOT MAKE OPTIMIZATION */
  *(uint16_t *)&(pPacket->FormatData.PacketData.RawData[pPacket->FormatData.length]) = pPacket->FormatData.crc16;
  if(pHandle->port_enable)
	  pHandle->port_tx(pPacket->RawData, pPacket->FormatData.length + 8);
}
#pragma GCC pop_options

/*
  decode process.
*/
void kylink_decode(KYLINK_CORE_HANDLE *pHandle, uint8_t data)
{
  switch(pHandle->decoder._decode_state) {
	case DECODE_STATE_UNSYNCED:
	  if(data == kySTX1) {
        pHandle->decoder._rx_packet.FormatData.stx1 = data;
        pHandle->decoder._decode_state = DECODE_STATE_GOT_STX1;
	  }
	break;
	case DECODE_STATE_GOT_STX1:
	  if(data == kySTX2) {
        pHandle->decoder._rx_packet.FormatData.stx2 = data;
        pHandle->decoder._decode_state = DECODE_STATE_GOT_STX2;
	  } else {
        pHandle->decoder._decode_state = DECODE_STATE_UNSYNCED;
	  }
	break;
	case DECODE_STATE_GOT_STX2:
      pHandle->decoder._rx_packet.FormatData.dev_id = data;
      pHandle->decoder._decode_state = DECODE_STATE_GOT_DEVID;
	break;
	case DECODE_STATE_GOT_DEVID:
      pHandle->decoder._rx_packet.FormatData.msg_id = data;
      pHandle->decoder._decode_state = DECODE_STATE_GOT_MSGID;
	break;
	case DECODE_STATE_GOT_MSGID:
      pHandle->decoder._rx_packet.FormatData.length = data;
      pHandle->decoder._decode_state = DECODE_STATE_GOT_LEN_L;
	break;
	case DECODE_STATE_GOT_LEN_L:
      pHandle->decoder._rx_packet.FormatData.length = ((uint16_t)data << 8) | pHandle->decoder._rx_packet.FormatData.length;
	  if(pHandle->decoder._rx_packet.FormatData.length <= MAIN_DATA_CACHE) {
        pHandle->decoder.rx_counter = 0;
		pHandle->decoder._decode_state = DECODE_STATE_GOT_LEN_H;
	  } else {
        pHandle->decoder._decode_state = DECODE_STATE_UNSYNCED;
	  }
	break;
	case DECODE_STATE_GOT_LEN_H:
      pHandle->decoder._rx_packet.FormatData.PacketData.RawData[pHandle->decoder.rx_counter ++] = data;
	  if(pHandle->decoder.rx_counter == pHandle->decoder._rx_packet.FormatData.length)
        pHandle->decoder._decode_state = DECODE_STATE_GOT_DATA;
	break;
	case DECODE_STATE_GOT_DATA:
      pHandle->decoder._rx_packet.FormatData.crc16 = data;
      pHandle->decoder._decode_state = DECODE_STATE_GOT_CRC_L;
	break;
	case DECODE_STATE_GOT_CRC_L:
      pHandle->decoder._rx_packet.FormatData.crc16 = (data << 8) | pHandle->decoder._rx_packet.FormatData.crc16; /* got the crc16. */
	  if(do_crc_table(&(pHandle->decoder._rx_packet.RawData[2]), pHandle->decoder.rx_counter + 4) == pHandle->decoder._rx_packet.FormatData.crc16) {
        pHandle->decoder._rx_packet_copy = pHandle->decoder._rx_packet;
        pHandle->decoder.update_flag = 1;
	  } else {}
      pHandle->decoder._decode_state = DECODE_STATE_UNSYNCED;
	break;
	default:
      pHandle->decoder._decode_state = DECODE_STATE_UNSYNCED;
	break;
  }
}

/**
 * @ End of file.
 */
