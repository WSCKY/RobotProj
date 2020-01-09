#include "server.h"
#include "kyLink.h"

static const char *TAG = "RTCM";

static ubx_npvts_t *p_npvts;
KYLINK_CORE_HANDLE kylink_server;
//static kyLinkPackageDef tx_packet;
static kyLinkConfig_t kylink_config;
static uint8_t kylink_cache[64];
static Location_T LocationInfo;

static uint8_t rover_to_server_step = 0;

uint8_t rtcm_cache[128];
uint32_t rtcm_cache_usage = 0;

static const char open_cmd[] =
  "AT+QIOPEN=1,0,\"TCP\",\""
  SERVER_IP_ADDR
  "\","
  SERVER_PORT
  ",0,2\r\n";

static void kylink_decode_cb(kyLinkBlockDef *pRx);

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
  rtcmif_init();
  ky_info(TAG, "ec20 & rtcm uart init.");
  if(ec20_check_ack((uint8_t *)"RDY\r\n", 8000) != status_ok)
	  ky_warn(TAG, "ec20 poweron check error.");
  else
	  ky_info(TAG, "ec20 powered.");
  osDelay(1000);
  ky_info(TAG, "wait f9p a/b stable.");
  while(check_rtk_rover_ready() == false) {
    osDelay(1000);
  }
  p_npvts = get_npvts_a();

  ky_info(TAG, "ec20 connect to server.");
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

  kylink_config.cache_size = 64;
  kylink_config.decoder_cache = kylink_cache;
  kylink_config.callback = kylink_decode_cb;
  kylink_config.txfunc = ec20if_tx_bytes;
  kylink_init(&kylink_server, &kylink_config);
//  kyLinkInit(&kylink_server);
//  kyLinkConfigTxFunc(&kylink_server, ec20if_tx_bytes);
//  kyLinkConfigCbFunc(&kylink_server, kylink_decode_cb);
//  kyLinkInitPackage(&tx_packet);

  for(;;) {
    if(rover_to_server_step <= 1) {
      osDelay(200);
      cnt = 0;
      rx_len = ec20if_rx_bytes(rx_cache, 32);
      while(cnt < rx_len) {
        kylink_decode(&kylink_server, rx_cache[cnt ++]);
      }
    } else {
      osDelay(50);
      uint32_t usage = ec20if_cache_usage();
      if(usage != 0) rtcm_cache_usage = usage;
      while((rx_len = ec20if_rx_bytes(rtcm_cache, 128)) > 0) {
        rtcmif_tx_bytes_util(rtcm_cache, rx_len);
      }
    }

    if(osKernelSysTick() - tx_timestamp > (1000 + rover_to_server_step * 500)) {
      if(rover_to_server_step == 0) {
        ec20if_tx_string_util(open_cmd);
      } else {
//        SendTxPacket(&kylink_server, &tx_packet);
    	  LocationInfo.info_type = 0x01;
    	  LocationInfo.lon = p_npvts->lon;
    	  LocationInfo.lat = p_npvts->lat;
    	  kylink_send(&kylink_server, (void *)&LocationInfo, 0x61, 0x09);
      }
      tx_timestamp = osKernelSysTick();
    }
  }
}

static void kylink_decode_cb(kyLinkBlockDef *pRx)
{
  uint16_t _kylink_dev_msg_group = ((uint16_t)pRx->dev_id << 8) | pRx->msg_id;
  if(_kylink_dev_msg_group == 0x0001) {
    if(rover_to_server_step == 0) {
      rover_to_server_step ++;
      ky_info(TAG, "got response from server.");
    }
  }
  if(_kylink_dev_msg_group == 0x0061) {
//    if(pRx->FormatData.PacketData.RawData[0] == 0x01) {
    if(pRx->buffer[0] == 0x01) {
      if(rover_to_server_step == 1) {
        rover_to_server_step ++;
//        tx_packet.FormatData.msg_id = 0x61;
//        tx_packet.FormatData.length = 0x09;
//        tx_packet.FormatData.PacketData.TypeData.LocationInfo.info_type = 0x01;
//        tx_packet.FormatData.PacketData.TypeData.LocationInfo.lon = p_npvts->lon;
//        tx_packet.FormatData.PacketData.TypeData.LocationInfo.lat = p_npvts->lat;
        ky_info(TAG, "send postion info to server.");
      }
    }
  }
}
