#include "f9pconfig.h"

status_t f9p_uart_config(ubx_handle_t *hubx, uint8_t id, uint32_t baudrate, uint16_t inProto, uint16_t outProto)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x14) return status_error;
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x00;
  hubx->_tx_packet.len_l = 0x14;
  hubx->_tx_packet.len_h = 0x00;
  hubx->_tx_packet.payload[0] = id;
  hubx->_tx_packet.payload[1] = 0x00;
  hubx->_tx_packet.payload[2] = 0x00;
  hubx->_tx_packet.payload[3] = 0x00;
  hubx->_tx_packet.payload[4] = 0xD0;
  hubx->_tx_packet.payload[5] = 0x08;
  hubx->_tx_packet.payload[6] = 0x00;
  hubx->_tx_packet.payload[7] = 0x00;
  hubx->_tx_packet.payload[8] = baudrate & 0xFF;
  hubx->_tx_packet.payload[9] = (baudrate >> 8) & 0xFF;
  hubx->_tx_packet.payload[10] = (baudrate >> 16) & 0xFF;
  hubx->_tx_packet.payload[11] = (baudrate >> 24) & 0xFF;
  hubx->_tx_packet.payload[12] = inProto & 0xFF;
  hubx->_tx_packet.payload[13] = (inProto >> 8) & 0xFF;
  hubx->_tx_packet.payload[14] = outProto & 0xFF;
  hubx->_tx_packet.payload[15] = (outProto >> 8) & 0xFF;
  hubx->_tx_packet.payload[16] = 0x00;
  hubx->_tx_packet.payload[17] = 0x00;
  hubx->_tx_packet.payload[18] = 0x00;
  hubx->_tx_packet.payload[19] = 0x00;
  return ubx_send(hubx);
}

status_t f9p_get_uart_config(ubx_handle_t *hubx, uint8_t id)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x01) return status_error;
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x00;
  hubx->_tx_packet.len_l = 0x01;
  hubx->_tx_packet.len_h = 0x00;
  hubx->_tx_packet.payload[0] = id;
  return ubx_send(hubx);
}

status_t f9p_nav_rate_config(ubx_handle_t *hubx, uint16_t meas_rate, uint16_t nav_rate, uint16_t time_ref)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x06) return status_error;
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x08;
  hubx->_tx_packet.len_l = 0x06;
  hubx->_tx_packet.len_h = 0x00;
  hubx->_tx_packet.payload[0] = meas_rate & 0xFF;
  hubx->_tx_packet.payload[1] = (meas_rate >> 8) & 0xFF;
  hubx->_tx_packet.payload[2] = nav_rate & 0xFF;
  hubx->_tx_packet.payload[3] = (nav_rate >> 8) & 0xFF;
  hubx->_tx_packet.payload[4] = time_ref & 0xFF;
  hubx->_tx_packet.payload[5] = (time_ref >> 8) & 0xFF;
  return ubx_send(hubx);
}

status_t f9p_poll_msg_config(ubx_handle_t *hubx, uint8_t msg_class, uint8_t msg_id)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  hubx->_tx_packet.group = msg_class;
  hubx->_tx_packet.id = msg_id;
  hubx->_tx_packet.len_l = 0x00;
  hubx->_tx_packet.len_h = 0x00;
  return ubx_send(hubx);
}

status_t f9p_msg_rate_config(ubx_handle_t *hubx, uint8_t msg_class, uint8_t msg_id, uint8_t rate_hz)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x03) return status_error;
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x01;
  hubx->_tx_packet.len_l = 0x03;
  hubx->_tx_packet.len_h = 0x00;
  hubx->_tx_packet.payload[0] = msg_class;
  hubx->_tx_packet.payload[1] = msg_id;
  hubx->_tx_packet.payload[2] = rate_hz;
  return ubx_send(hubx);
}

status_t f9p_surveyin_config(ubx_handle_t *hubx, uint32_t mini_duration, uint32_t limit_in_m)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x28) return status_error;
  limit_in_m *= 10000; /* Survey-in position accuracy limit (m -> 0.1mm) */
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x71;
  hubx->_tx_packet.len_l = 0x28;
  hubx->_tx_packet.len_h = 0x00;
  for(uint32_t i = 0; i < 40; i ++)
    hubx->_tx_packet.payload[i] = 0x00;
  hubx->_tx_packet.payload[2] = 0x01; // mode -> survey-in
  hubx->_tx_packet.payload[24] = mini_duration & 0xFF;
  hubx->_tx_packet.payload[25] = (mini_duration >> 8) & 0xFF;
  hubx->_tx_packet.payload[26] = (mini_duration >> 16) & 0xFF;
  hubx->_tx_packet.payload[27] = (mini_duration >> 24) & 0xFF;
  hubx->_tx_packet.payload[28] = limit_in_m & 0xFF;
  hubx->_tx_packet.payload[29] = (limit_in_m >> 8) & 0xFF;
  hubx->_tx_packet.payload[30] = (limit_in_m >> 16) & 0xFF;
  hubx->_tx_packet.payload[31] = (limit_in_m >> 24) & 0xFF;
  return ubx_send(hubx);
}

/*
CFG-MSGOUT-RTCM_3X_TYPE1005_UART2 (KeyID 209102BF, 0x01)
CFG-MSGOUT-RTCM_3X_TYPE1074_UART2 (KeyID 20910360, 0x02)
CFG-MSGOUT-RTCM_3X_TYPE1084_UART2 (KeyID 20910365, 0x03)
CFG-MSGOUT-RTCM_3X_TYPE1094_UART2 (KeyID 2091036A, 0x04)
CFG-MSGOUT-RTCM_3X_TYPE1124_UART2 (KeyID 2091036F, 0x05)
CFG-MSGOUT-RTCM_3X_TYPE1230_UART2 (KeyID 20910305, 0x06)
B5 62 06 8A 22 00 00 01 00 00 BF 02 91 20 01 60 03 91 20 02 65 03 91 20 03 6A 03 91 20 04 6F 03 91 20 05 05 03 91 20 06 61 67
*/
static const uint8_t rtcm3x_uart2_msgs[] = { 0x00, 0x01, 0x00, 0x00,
                                             0xBF, 0x02, 0x91, 0x20, 0x01,
                                             0x60, 0x03, 0x91, 0x20, 0x02,
                                             0x65, 0x03, 0x91, 0x20, 0x03,
                                             0x6A, 0x03, 0x91, 0x20, 0x04,
                                             0x6F, 0x03, 0x91, 0x20, 0x05,
                                             0x05, 0x03, 0x91, 0x20, 0x06};
status_t f9p_rtcm_msg_output_config(ubx_handle_t *hubx)
{
  if(hubx->tx_bytes == UBX_NULL) return status_error;
  if(hubx->_tx_payload_size < 0x22) return status_error;
  hubx->_tx_packet.group = 0x06;
  hubx->_tx_packet.id = 0x8A;
  hubx->_tx_packet.len_l = 0x22;
  hubx->_tx_packet.len_h = 0x00;
  for(uint32_t i = 0; i < 0x22; i ++)
    hubx->_tx_packet.payload[i] = rtcm3x_uart2_msgs[i];
  return ubx_send(hubx);
}
