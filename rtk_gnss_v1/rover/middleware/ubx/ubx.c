#include "ubx.h"

static void ubx_compsum(ubx_struct_t *ubx);
static status_t ubx_verify(ubx_struct_t *ubx);

status_t ubx_init(ubx_handle_t *hubx)
{
  hubx->tx_bytes = UBX_NULL;
  hubx->_rx_payload_size = 0;
  hubx->_tx_payload_size = 0;
  hubx->_rx_packet.payload = UBX_NULL;
  hubx->_tx_packet.payload = UBX_NULL;
  hubx->_tx_packet.s1 = UBX_SYN_STX1;
  hubx->_tx_packet.s2 = UBX_SYN_STX2;
  hubx->_tx_packet.ck_a = 0;
  hubx->_tx_packet.ck_b = 0;
  return status_ok;
}

void ubx_config_tx_func(ubx_handle_t *hubx, ubxTxBytesFunc pTx)
{
  hubx->tx_bytes = pTx;
}

void ubx_config_cb_func(ubx_handle_t *hubx, ubxDecodeCallback pcb)
{
  hubx->decoder.decode_callback = pcb;
}

void ubx_config_payload(ubx_handle_t *hubx, uint8_t *ptx, uint16_t tsize, uint8_t *prx, uint16_t rsize)
{
  hubx->_tx_packet.payload = ptx;
  hubx->_rx_packet.payload = prx;
  hubx->_tx_payload_size = tsize;
  hubx->_rx_payload_size = rsize;
}

status_t ubx_send(ubx_handle_t *hubx)
{
  status_t ret = status_error;
  ubx_compsum(&hubx->_tx_packet);
  if(hubx->tx_bytes != UBX_NULL) {
    ret = hubx->tx_bytes((uint8_t *)&hubx->_tx_packet, 6); if(ret != status_ok) return ret;
    ret = hubx->tx_bytes((uint8_t *)hubx->_tx_packet.payload, (hubx->_tx_packet.len_h << 8) + hubx->_tx_packet.len_l); if(ret != status_ok) return ret;
    ret = hubx->tx_bytes((uint8_t *)&hubx->_tx_packet.ck_a, 2); if(ret != status_ok) return ret;
  }
  return ret;
}

void ubx_decoder_reset(ubx_handle_t *hubx)
{
  hubx->decoder._decode_state = UBX_DECODE_UNSYNCED;
  hubx->decoder._rx_counter = 0;
  hubx->decoder._rx_length = 0;
}

void ubx_decode(ubx_handle_t *hubx, uint8_t data)
{
  switch(hubx->decoder._decode_state) {
    case UBX_DECODE_UNSYNCED:
      if(data == UBX_SYN_STX1) {
        hubx->_rx_packet.s1 = data;
        hubx->decoder._decode_state = UBX_DECODE_GOT_SYN1;
      }
      break;
    case UBX_DECODE_GOT_SYN1:
      if(data == UBX_SYN_STX2) {
        hubx->_rx_packet.s2 = data;
        hubx->decoder._decode_state = UBX_DECODE_GOT_SYN2;
      } else {
        hubx->decoder._decode_state = UBX_DECODE_UNSYNCED;
      }
      break;
    case UBX_DECODE_GOT_SYN2:
      hubx->_rx_packet.group = data;
      hubx->decoder._decode_state = UBX_DECODE_GOT_CLASS;
      break;
    case UBX_DECODE_GOT_CLASS:
      hubx->_rx_packet.id = data;
      hubx->decoder._decode_state = UBX_DECODE_GOT_ID;
      break;
    case UBX_DECODE_GOT_ID:
      hubx->_rx_packet.len_l = data;
      hubx->decoder._rx_length = data;
      hubx->decoder._decode_state = UBX_DECODE_GOT_LEN_L;
      break;
    case UBX_DECODE_GOT_LEN_L:
      hubx->_rx_packet.len_h = data;
      hubx->decoder._rx_length += (uint16_t)data << 8;
      if(hubx->decoder._rx_length > hubx->_rx_payload_size) {
        hubx->decoder._rx_length = 0;
        hubx->decoder._decode_state = UBX_DECODE_UNSYNCED;
      } else {
        hubx->decoder._rx_counter = 0;
        hubx->decoder._decode_state = UBX_DECODE_GOT_LEN_H;
      }
      break;
    case UBX_DECODE_GOT_LEN_H:
      hubx->_rx_packet.payload[hubx->decoder._rx_counter ++] = data;
      if(hubx->decoder._rx_counter >= hubx->decoder._rx_length) {
        hubx->decoder._rx_counter = 0;
        hubx->decoder._decode_state = UBX_DECODE_GOT_DATA;
      }
      break;
    case UBX_DECODE_GOT_DATA:
      hubx->_rx_packet.ck_a = data;
      hubx->decoder._decode_state = UBX_DECODE_GOT_CK_A;
      break;
    case UBX_DECODE_GOT_CK_A:
      hubx->_rx_packet.ck_b = data;
      hubx->decoder._decode_state = UBX_DECODE_UNSYNCED;
      if(ubx_verify(&hubx->_rx_packet) == status_ok) {
        if(hubx->decoder.decode_callback != UBX_NULL)
          hubx->decoder.decode_callback(&hubx->_rx_packet);
      }
      break;
  }
}

static void ubx_compsum(ubx_struct_t *ubx)
{
  uint16_t i = 0, len = (ubx->len_h << 8) + ubx->len_l;
  uint8_t *p = (uint8_t *)&(ubx->group);
  ubx->ck_a = 0;
  ubx->ck_b = 0;
  for(i = 0; i < 4; i ++) {
    ubx->ck_a += p[i];
    ubx->ck_b += ubx->ck_a;
  }
  p = ubx->payload;
  for(i = 0; i < len; i ++) {
    ubx->ck_a += p[i];
    ubx->ck_b += ubx->ck_a;
  }
}

static status_t ubx_verify(ubx_struct_t *ubx)
{
  uint8_t ck_a = 0, ck_b = 0;
  uint16_t i = 0, len = (ubx->len_h << 8) + ubx->len_l;
  uint8_t *p = (uint8_t *)&(ubx->group);
  for(i = 0; i < 4; i ++) {
    ck_a += p[i];
    ck_b += ck_a;
  }
  p = ubx->payload;
  for(i = 0; i < len; i ++) {
    ck_a += p[i];
    ck_b += ck_a;
  }
  if((ck_a == ubx->ck_a) && ck_b == ubx->ck_b) return status_ok;
  return status_error;
}
