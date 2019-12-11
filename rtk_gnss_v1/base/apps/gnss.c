#include "gnss.h"
#include "kyLink.h"
#include <stdio.h>

#include "f9pconfig.h"

osThreadId decodeThreadHandle, rtcmThreadHandle, f9pThreadHandle_a, f9pThreadHandle_b;

ubx_handle_t gnss_f9p_a, gnss_f9p_b;

static uint8_t ubx_tx_payload_a[64];
static uint8_t ubx_tx_payload_b[64];
static uint8_t ubx_rx_payload_a[96];
static uint8_t ubx_rx_payload_b[96];

static uint16_t _ubx_ack_ack_a, _ubx_ack_ack_b;
static ubx_npvts_t gnss_npvts_a, gnss_npvts_b;

static uint8_t little_endian_1b(uint8_t *p);
static uint16_t little_endian_2b(uint8_t *p);
static uint32_t little_endian_4b(uint8_t *p);
static void decode_npvts(ubx_npvts_t *p_npvts, uint8_t *p_payload);

static void f9p_proc_task_a(void const *argument);
static void f9p_proc_task_b(void const *argument);
static void ubx_decode_task(void const *argument);
static void ubxa_decode_callback(ubx_struct_t *p);
static void ubxb_decode_callback(ubx_struct_t *p);

void gnss_navg_task(void const *argument)
{
  ubxaif_init();
  ubxbif_init();
  ubx_init(&gnss_f9p_a);
  ubx_init(&gnss_f9p_b);
  ubx_config_tx_func(&gnss_f9p_a, ubxaif_tx_bytes_util);
  ubx_config_tx_func(&gnss_f9p_b, ubxbif_tx_bytes_util);
  ubx_config_cb_func(&gnss_f9p_a, ubxa_decode_callback);
  ubx_config_cb_func(&gnss_f9p_b, ubxb_decode_callback);
  ubx_config_payload(&gnss_f9p_a, ubx_tx_payload_a, 64, ubx_rx_payload_a, 96);
  ubx_config_payload(&gnss_f9p_b, ubx_tx_payload_b, 64, ubx_rx_payload_b, 96);

#if 1
  ubxaif_reset_enable();
  ubxbif_reset_enable();
  osDelay(1000);
  ubxaif_reset_disable();
  ubxbif_reset_disable();
#endif
  osDelay(1000); // wait for f9p ready

  dbg_str("f9p if init ok.\n");

  osThreadDef(F9PA, f9p_proc_task_a, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(F9PB, f9p_proc_task_b, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(UBX_DECODE, ubx_decode_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadDef(RTCM_TRANS, rtcm_transfer_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  f9pThreadHandle_a = osThreadCreate(osThread(F9PA), NULL);
  if(f9pThreadHandle_a == NULL) dbg_str("f9p proc task a failed.\n");
  f9pThreadHandle_b = osThreadCreate(osThread(F9PB), NULL);
  if(f9pThreadHandle_b == NULL) dbg_str("f9p proc task b failed.\n");
  decodeThreadHandle = osThreadCreate(osThread(UBX_DECODE), NULL);
  if(decodeThreadHandle == NULL) dbg_str("f9p decode task failed.\n");
  rtcmThreadHandle = osThreadCreate(osThread(RTCM_TRANS), &gnss_npvts_a);
  if(rtcmThreadHandle == NULL) dbg_str("rtcm transfer task failed.\n");

  vTaskDelete(NULL);
}

static bool_t rtk_base_ready = false;

bool_t check_rtk_base_ready(void)
{
  return rtk_base_ready;
}

static bool_t gnss_npvts_update;
static uint8_t f9pa_svin_in_prog = 0, f9pa_svin_validity = 0;
uint32_t f9pa_svin_in_obs = 0, f9pa_svin_mean_acc = 0;//static 
static void f9p_proc_task_a(void const *argument)
{
  uint16_t _ubx_ack_ack_exp;
  uint32_t config_cnt_a, check_cnt_a;
  dbg_str("f9p a uart1 config.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_PRT;
  config_cnt_a = 3;
  while(config_cnt_a --) {
    f9p_uart_config(&gnss_f9p_a, 1, 230400, UBLOX_PROTO_UBX | UBLOX_PROTO_RTCM3, UBLOX_PROTO_UBX);
    ubxaif_set_baudrate(230400);
    osDelay(200);
    ubx_decoder_reset(&gnss_f9p_a);
    _ubx_ack_ack_a = 0x0000;
    check_cnt_a = 2;
    while(_ubx_ack_ack_exp != _ubx_ack_ack_a && check_cnt_a --) {
      f9p_get_uart_config(&gnss_f9p_a, 1);
      osDelay(500);
    }
    if(_ubx_ack_ack_exp == _ubx_ack_ack_a) {
      dbg_str("f9p a uart1 config done.\n");
      break; // config done.
    } else {
      ubxaif_set_baudrate(38400);
    }
  }
//  dbg_str("f9p a set rate to 10Hz.\n");
//  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_RATE;
//  _ubx_ack_ack_a = 0x0000;
//  config_cnt_a = 3;
//  while(config_cnt_a --) {
//    f9p_nav_rate_config(&gnss_f9p_a, 100, 1, TIME_SYSTEM_UTC); // measure rate: 10Hz(100ms), nav rate: 1 cycle, time_ref: UTC
//    osDelay(500);
//    if(_ubx_ack_ack_a == _ubx_ack_ack_exp) {
//      dbg_str("f9p a rate config done.\n");
//      break;
//    }
//  }
/* - Survey-in mode for base - */
  dbg_str("f9p a survey-in 2m,300s.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_TMODE3;
  _ubx_ack_ack_a = 0x0000;
  config_cnt_a = 3;
  while(config_cnt_a --) {
    f9p_surveyin_config(&gnss_f9p_a, 300, 2);
    osDelay(500);
    if(_ubx_ack_ack_a == _ubx_ack_ack_exp) {
      dbg_str("f9p a svin config done.\n");
      break;
    }
  }

  dbg_str("f9p a poll svin status.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_MSG;
  _ubx_ack_ack_a = 0x0000;
  config_cnt_a = 3;
  while(config_cnt_a --) {
    f9p_msg_rate_config(&gnss_f9p_a, UBX_NAV, UBX_NAV_SVIN, 1);
    osDelay(500);
    if(_ubx_ack_ack_a == _ubx_ack_ack_exp) {
      dbg_str("f9p a svin msg enabled.\n");
      break;
    }
  }

  while(f9pa_svin_in_prog == 0 && f9pa_svin_validity == 0) {
    osDelay(1000);
  }
  dbg_str("f9p a svin in progress.\n");
  while(f9pa_svin_validity == 0) {
    osDelay(1000);
  }
  dbg_str("f9p a svin successfully.\n");

  dbg_str("f9p a close svin message.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_MSG;
  _ubx_ack_ack_a = 0x0000;
  config_cnt_a = 3;
  while(config_cnt_a --) {
    f9p_msg_rate_config(&gnss_f9p_a, UBX_NAV, UBX_NAV_SVIN, 0);
    osDelay(500);
    if(_ubx_ack_ack_a == _ubx_ack_ack_exp) {
      dbg_str("f9p a svin msg closed.\n");
      break;
    }
  }
/* - Survey-in mode for base - */
  gnss_npvts_update = false;
  while(gnss_npvts_update == false) {
    f9p_poll_msg_config(&gnss_f9p_a, UBX_NAV, UBX_NAV_PVT);
    osDelay(1000);
  }
  dbg_str("f9p a got position.\n");
  rtcmif_init(); // start transmit/receive RTCM3.x message.
  dbg_str("f9p a rtcm port initialized.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_VALSET;
  _ubx_ack_ack_a = 0x0000;
  config_cnt_a = 3;
  while(config_cnt_a --) {
    f9p_rtcm_msg_output_config(&gnss_f9p_a);
    osDelay(500);
    if(_ubx_ack_ack_a == _ubx_ack_ack_exp) {
      dbg_str("f9p a rtcm msg enabled.\n");
      break;
    }
  }

  rtk_base_ready = true;
//  if(rtcmThreadHandle != NULL)
//    osThreadResume(rtcmThreadHandle);
  dbg_str("f9p a running ...\n");

  vTaskDelete(NULL);
}

static uint8_t f9pb_str[128];
static void f9p_proc_task_b(void const *argument)
{
  uint16_t _ubx_ack_ack_exp;
  uint32_t config_cnt_b, check_cnt_b;
  dbg_str("f9p b port config.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_PRT;
  config_cnt_b = 3;
  while(config_cnt_b --) {
    f9p_uart_config(&gnss_f9p_b, 1, 230400, UBLOX_PROTO_UBX, UBLOX_PROTO_UBX);
    ubxbif_set_baudrate(230400);
    dbg_str("f9p b sent port config.\n");
    osDelay(200);
    ubx_decoder_reset(&gnss_f9p_b);
    _ubx_ack_ack_b = 0x0000;
    check_cnt_b = 2;
    while(_ubx_ack_ack_exp != _ubx_ack_ack_b && check_cnt_b --) {
      f9p_get_uart_config(&gnss_f9p_b, 1);
      dbg_str("f9p b check port config.\n");
      osDelay(500);
    }
    if(_ubx_ack_ack_exp == _ubx_ack_ack_b) {
      dbg_str("f9p b port config done.\n");
      break; // config done.
    } else {
      ubxbif_set_baudrate(38400);
    }
  }
  dbg_str("f9p b set rate to 10Hz.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_RATE;
  _ubx_ack_ack_b = 0x0000;
  config_cnt_b = 3;
  while(config_cnt_b --) {
    f9p_nav_rate_config(&gnss_f9p_b, 50, 1, TIME_SYSTEM_UTC); // measure rate: 10Hz(100ms), nav rate: 1 cycle, time_ref: UTC
    osDelay(500);
    if(_ubx_ack_ack_b == _ubx_ack_ack_exp) {
      dbg_str("f9p b rate config done.\n");
      break;
    }
  }

  dbg_str("f9p b poll npvts packet.\n");
  _ubx_ack_ack_exp = ((uint16_t)UBX_CFG << 8) + UBX_CFG_MSG;
  _ubx_ack_ack_b = 0x0000;
  config_cnt_b = 3;
  while(config_cnt_b --) {
    f9p_msg_rate_config(&gnss_f9p_b, UBX_NAV, UBX_NAV_PVT, 1);
    osDelay(500);
    if(_ubx_ack_ack_b == _ubx_ack_ack_exp) {
      dbg_str("f9p b npvts msg enabled.\n");
      break;
    }
  }

  dbg_str("f9p b running ...\n");
//  vTaskDelete(NULL);
  for(;;) {
    osDelay(2000);
    sprintf((char *)f9pb_str, "f9p b -> fix type: %d, flags: 0x%02x.\n", gnss_npvts_b.fix_type, gnss_npvts_b.flags);
    dbg_str((const char *)f9pb_str);
  }
}

static void ubx_decode_task(void const *argument)
{
  uint8_t rx_cache[32];
  uint32_t rx_len = 0, cnt = 0;
  dbg_str("f9p decode task.\n");
  for(;;) {
    osDelay(5);
    cnt = 0;
    rx_len = ubxaif_rx_bytes(rx_cache, 32);
    while(cnt < rx_len) {
      ubx_decode(&gnss_f9p_a, rx_cache[cnt ++]);
    }
    cnt = 0;
    rx_len = ubxbif_rx_bytes(rx_cache, 32);
    while(cnt < rx_len) {
      ubx_decode(&gnss_f9p_b, rx_cache[cnt ++]);
    }
  }
}

static void ubxa_decode_callback(ubx_struct_t *p)
{
  if(p->group == UBX_ACK && p->id == UBX_ACK_ACK) {
    _ubx_ack_ack_a = ((uint16_t)p->payload[0] << 8) + p->payload[1];
  } else if(p->group == UBX_CFG && p->id == UBX_CFG_PRT) {
//    _baud_rate_port = p->payload[8] + ((uint32_t)p->payload[9] << 8) + ((uint32_t)p->payload[10] << 16) + ((uint32_t)p->payload[11] << 24);
  } else if(p->group == UBX_NAV && p->id == UBX_NAV_SVIN) {
    f9pa_svin_mean_acc = little_endian_4b(p->payload + 28);
    f9pa_svin_in_obs = little_endian_4b(p->payload + 32);
    f9pa_svin_validity = p->payload[36];
    f9pa_svin_in_prog = p->payload[37];
  } else if(p->group == UBX_NAV && p->id == UBX_NAV_PVT) {
    decode_npvts(&gnss_npvts_a, p->payload);
    gnss_npvts_update = true;
  }
}

static void ubxb_decode_callback(ubx_struct_t *p)
{
  if(p->group == UBX_ACK && p->id == UBX_ACK_ACK) {
    _ubx_ack_ack_b = ((uint16_t)p->payload[0] << 8) + p->payload[1];
  } else if(p->group == UBX_NAV && p->id == UBX_NAV_PVT) {
    decode_npvts(&gnss_npvts_b, p->payload);
  }
}

static void decode_npvts(ubx_npvts_t *p_npvts, uint8_t *p_payload)
{
  p_npvts->i_tow = little_endian_4b(p_payload + 0);
  p_npvts->year = little_endian_2b(p_payload + 4);
  p_npvts->month = little_endian_1b(p_payload + 6);
  p_npvts->day = little_endian_1b(p_payload + 7);
  p_npvts->hour = little_endian_1b(p_payload + 8);
  p_npvts->min = little_endian_1b(p_payload + 9);
  p_npvts->sec = little_endian_1b(p_payload + 10);
  p_npvts->valid = little_endian_1b(p_payload + 11);
  p_npvts->t_acc = little_endian_4b(p_payload + 12);
  p_npvts->nano = little_endian_4b(p_payload + 16);
  p_npvts->fix_type = little_endian_1b(p_payload + 20);
  p_npvts->flags = little_endian_1b(p_payload + 21);
  p_npvts->flags2 = little_endian_1b(p_payload + 22);
  p_npvts->num_sv = little_endian_1b(p_payload + 23);
  p_npvts->lon = little_endian_4b(p_payload + 24);
  p_npvts->lat = little_endian_4b(p_payload + 28);
  p_npvts->height = little_endian_4b(p_payload + 32);
  p_npvts->h_msl = little_endian_4b(p_payload + 36);
  p_npvts->h_acc = little_endian_4b(p_payload + 40);
  p_npvts->v_acc = little_endian_4b(p_payload + 44);
  p_npvts->vel_n = little_endian_4b(p_payload + 48);
  p_npvts->vel_e = little_endian_4b(p_payload + 52);
  p_npvts->vel_d = little_endian_4b(p_payload + 56);
  p_npvts->g_speed = little_endian_4b(p_payload + 60);
  p_npvts->head_mot = little_endian_4b(p_payload + 64);
  p_npvts->s_acc = little_endian_4b(p_payload + 68);
  p_npvts->head_acc = little_endian_4b(p_payload + 72);
  p_npvts->p_dop = little_endian_2b(p_payload + 76);
  p_npvts->head_veh = little_endian_4b(p_payload + 84);
  p_npvts->mag_dec = little_endian_2b(p_payload + 88);
  p_npvts->mag_acc = little_endian_2b(p_payload + 90);
}

static uint8_t little_endian_1b(uint8_t *p)
{
  return p[0];
}

static uint16_t little_endian_2b(uint8_t *p)
{
  return ((uint16_t)p[1] << 8) + p[0];
}

static uint32_t little_endian_4b(uint8_t *p)
{
  return ((uint32_t)p[3] << 24) + ((uint32_t)p[2] << 16) + ((uint32_t)p[1] << 8) + p[0];
}
