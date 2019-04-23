#include "kyLink.h"

#if FREERTOS_ENABLED
QueueHandle_t lnk_recv_q = NULL;
static void lnk_queue_create(void);
#else
static uint8_t GotDataFlag = 0;
static CommPackageDef rxPacket = {0};
#endif /* FREERTOS_ENABLED */

/* function prototypes */
static uint16_t do_crc_table(uint8_t *ptr, uint32_t len);

void kyLink_Init(void)
{
#if FREERTOS_ENABLED
  lnk_queue_create();
#else
  GotDataFlag = 0;
#endif /* FREERTOS_ENABLED */
}

#if FREERTOS_ENABLED
static void lnk_queue_create(void)
{
  lnk_recv_q = xQueueCreate(MSG_QUEUE_DEPTH, sizeof(CommPackageDef));
}
#else
uint8_t GotNewData(void)
{
	if(GotDataFlag == 1) {
		GotDataFlag = 0;
		return 1;
	}
	return 0;
}

CommPackageDef* GetRxPacket(void)
{
	return &rxPacket;
}
#endif /* FREERTOS_ENABLED */

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

void SendTxPacket(CommPackageDef* pPacket)
{
	pPacket->Packet.crc16 = do_crc_table(&(pPacket->RawData[2]), pPacket->Packet.length + 4);
  *(uint16_t *)&(pPacket->Packet.PacketData.pData[pPacket->Packet.length]) = pPacket->Packet.crc16;
  if(COM_IF_TX_CHECK())
    COM_IF_TX_BYTES(pPacket->RawData, pPacket->Packet.length + 8);
}

/* decoder required. */
static uint8_t _rx_length = 0;
static CommPackageDef _rx_packet = {0};
static DECODE_STATE _decode_state = DECODE_STATE_UNSYNCED;
/*
  decode process.
*/
void kyLink_DecodeProcess(uint8_t data)
{
#if FREERTOS_ENABLED
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
	switch(_decode_state) {
		case DECODE_STATE_UNSYNCED:
			if(data == kySTX1) {
				_rx_packet.Packet.stx1 = data;
				_decode_state = DECODE_STATE_GOT_STX1;
			}
		break;
		case DECODE_STATE_GOT_STX1:
			if(data == kySTX2) {
				_rx_packet.Packet.stx2 = data;
				_decode_state = DECODE_STATE_GOT_STX2;
			} else {
				_decode_state = DECODE_STATE_UNSYNCED;
      }
		break;
    case DECODE_STATE_GOT_STX2:
      _rx_packet.Packet.dev_id = data;
      _decode_state = DECODE_STATE_GOT_DEVID;
    break;
    case DECODE_STATE_GOT_DEVID:
      _rx_packet.Packet.msg_id = data;
      _decode_state = DECODE_STATE_GOT_MSGID;
    break;
    case DECODE_STATE_GOT_MSGID:
      _rx_packet.Packet.length = data;
      _decode_state = DECODE_STATE_GOT_LEN_L;
    break;
    case DECODE_STATE_GOT_LEN_L:
      _rx_packet.Packet.length = ((uint16_t)data << 8) | _rx_packet.Packet.length;
      if(_rx_packet.Packet.length <= MAIN_DATA_CACHE) {
        _rx_length = 0;
        _decode_state = DECODE_STATE_GOT_LEN_H;
      } else {
        _decode_state = DECODE_STATE_UNSYNCED;
      }
    break;
		case DECODE_STATE_GOT_LEN_H:
			_rx_packet.Packet.PacketData.pData[_rx_length ++] = data;
			if(_rx_length == _rx_packet.Packet.length)
				_decode_state = DECODE_STATE_GOT_DATA;
		break;
		case DECODE_STATE_GOT_DATA:
			_rx_packet.Packet.crc16 = data;
      _decode_state = DECODE_STATE_GOT_CRC_L;
    break;
    case DECODE_STATE_GOT_CRC_L:
      _rx_packet.Packet.crc16 = (data << 8) | _rx_packet.Packet.crc16; /* got the crc16. */
			if(do_crc_table(&(_rx_packet.RawData[2]), _rx_length + 4) == _rx_packet.Packet.crc16) {
#if FREERTOS_ENABLED
        xQueueSendFromISR(lnk_recv_q, &_rx_packet, &xHigherPriorityTaskWoken);
#else
        rxPacket = _rx_packet;
        GotDataFlag = 1;
#endif /* FREERTOS_ENABLED */
			} else {}
			_decode_state = DECODE_STATE_UNSYNCED;
		break;
		default:
			_decode_state = DECODE_STATE_UNSYNCED;
		break;
	}
}
