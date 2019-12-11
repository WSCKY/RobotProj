#ifndef __UBX_H
#define __UBX_H

#include "SysConfig.h"

#define UBX_SYN_STX1                   (0xB5)
#define UBX_SYN_STX2                   (0x62)

#define UBX_NULL                       (0U)

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

__PACK_BEGIN typedef struct
{
	uint8_t s1, s2;
	uint8_t group, id;
	uint8_t len_l, len_h;
	uint8_t* payload;
	uint8_t ck_a, ck_b;
} __PACK_END ubx_struct_t;

typedef status_t (*ubxTxBytesFunc)(uint8_t *, uint32_t);
typedef void (*ubxDecodeCallback)(ubx_struct_t *);

__PACK_BEGIN typedef struct
{
  uint16_t _rx_length;
  uint16_t _rx_counter;
  UBX_DECODE_STATE _decode_state;
  ubxDecodeCallback decode_callback;
} __PACK_END ubx_decoder_t;

__PACK_BEGIN typedef struct
{
  ubx_decoder_t decoder;
  ubx_struct_t _rx_packet;
  ubx_struct_t _tx_packet;
  uint16_t _rx_payload_size;
  uint16_t _tx_payload_size;
  ubxTxBytesFunc tx_bytes;
} __PACK_END ubx_handle_t;

void ubx_config_tx_func(ubx_handle_t *hubx, ubxTxBytesFunc pTx);
void ubx_config_cb_func(ubx_handle_t *hubx, ubxDecodeCallback pcb);
void ubx_config_payload(ubx_handle_t *hubx, uint8_t *ptx, uint16_t tsize, uint8_t *prx, uint16_t rsize);

status_t ubx_init(ubx_handle_t *hubx);
status_t ubx_send(ubx_handle_t *hubx);
void ubx_decoder_reset(ubx_handle_t *hubx);
void ubx_decode(ubx_handle_t *hubx, uint8_t data);

#endif /* __UBX_H */
