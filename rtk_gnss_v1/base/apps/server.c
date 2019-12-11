#include "gnss.h"
#include "kyLink.h"
#include "f9pconfig.h"

KYLINK_CORE_HANDLE kylink_server;

static kyLinkPackageDef tx_packet;
static bool_t rtcm_msg_tx_ready = false;

static uint8_t base_to_server_step = 0;
static int32_t base_lon = 0, base_lat = 0;

uint8_t rtcm_cache[128];
uint32_t rtcm_cache_usage = 0;

#define SERVER_IP_ADDR                           "36.153.88.121"
#define SERVER_PORT                              "8086"

static const char open_cmd[] =
  "AT+QIOPEN=1,0,\"TCP\",\""
  SERVER_IP_ADDR
  "\","
  SERVER_PORT
  ",0,2\r\n";

static void kylink_decode_cb(kyLinkPackageDef *pRx);

void rtcm_transfer_task(void const *argument)
{
//  bool_t ret;
  uint8_t rx_cache[32];
  uint32_t rx_len = 0, cnt = 0;
  uint32_t tx_timestamp = 0;

  ec20_power_init();
  ec20_power_on();
  osDelay(3000);
  ec20if_init();
  dbg_str("ec20 uart init.\n");
  ec20_check_ack((uint8_t *)"RDY\r\n", 8000);
  dbg_str("ec20 powered.\n");

  while(check_rtk_base_ready() == false) {
    osDelay(1000);
  }
  dbg_str("ec20 connect to server.\n");
//  ret = false;
//  if(ec20_check_cmd_ack((uint8_t *)"AT\r\n", (uint8_t *)"AT\r\r\nOK\r\n", 5, 1000) == status_ok) {
//    dbg_str("ec20 test done.\n");
//    osDelay(1000);
//    if(ec20_check_cmd_ack((uint8_t *)"AT+QIOPEN=1,0,\"TCP\",\"36.153.88.121\",8086,0,2\r\n", (uint8_t *)"\r\r\nCONNECT\r\n", 5, 1000) == status_ok) {
//      dbg_str("ec20 start done.\n");
//      ret = true;
//    } else
//      dbg_str("ec20 start fail.\n");
//  } else {
//    dbg_str("ec20 test fail.\n");
//  }

//  while(ret == false) {
//    osDelay(2000);
//    dbg_str("server connect failed.\n");
//  }

//  ec20_check_cmd_ack((uint8_t *)"AT+QIOPEN=1,0,\"TCP\",\"36.153.88.121\",8086,0,2\r\n", (uint8_t *)"\r\r\nCONNECT\r\n", 5, 1000);
  ec20_check_cmd_ack((uint8_t *)open_cmd, (uint8_t *)"\r\r\nCONNECT\r\n", 5, 1000);

  base_lon = ((ubx_npvts_t *)argument)->lon;
  base_lat = ((ubx_npvts_t *)argument)->lat;

  kyLinkInit(&kylink_server);
  kyLinkConfigTxFunc(&kylink_server, ec20if_tx_bytes);
  kyLinkConfigCbFunc(&kylink_server, kylink_decode_cb);
  kyLinkInitPackage(&tx_packet);

  for(;;) {
    if(rtcm_msg_tx_ready == true) {
      osDelay(50);
      uint32_t usage = rtcmif_cache_usage();
      if(usage != 0) rtcm_cache_usage = usage;
      while((rx_len = rtcmif_rx_bytes(rtcm_cache, 128)) > 0) {
        ec20if_tx_bytes_util(rtcm_cache, rx_len);
      }
    } else {
      osDelay(200);
      cnt = 0;
      rx_len = ec20if_rx_bytes(rx_cache, 32);
      while(cnt < rx_len) {
        kylink_decode(&kylink_server, rx_cache[cnt ++]);
      }
      if(osKernelSysTick() - tx_timestamp > 1500) {
        if(base_to_server_step == 0)
          ec20if_tx_string_util(open_cmd);
        else
          SendTxPacket(&kylink_server, &tx_packet);
      }
    }
  }
}

static void kylink_decode_cb(kyLinkPackageDef *pRx)
{
  uint16_t _kylink_dev_msg_group = ((uint16_t)pRx->FormatData.dev_id << 8) | pRx->FormatData.msg_id;
  if(_kylink_dev_msg_group == 0x0001) {
    if(base_to_server_step == 0) {
      base_to_server_step ++;
      dbg_str("got response from server.\n");
    }
    if(base_to_server_step == 2) {
      if(rtcm_msg_tx_ready == false) {
        rtcm_msg_tx_ready = true;
        base_to_server_step ++;
        dbg_str("join to server successfully.\n");
      }
    }
  }
  if(_kylink_dev_msg_group == 0x0061) {
    if(pRx->FormatData.PacketData.RawData[0] == 0x01) { // request the location.
      if(base_to_server_step == 1) {
        base_to_server_step ++;
        tx_packet.FormatData.msg_id = 0x61;
        tx_packet.FormatData.length = 0x09;
        tx_packet.FormatData.PacketData.TypeData.LocationInfo.info_type = 0x01;
        tx_packet.FormatData.PacketData.TypeData.LocationInfo.lon = base_lon;
        tx_packet.FormatData.PacketData.TypeData.LocationInfo.lat = base_lat;
        dbg_str("send postion info to server.\n");
      }
    }
  }
}
