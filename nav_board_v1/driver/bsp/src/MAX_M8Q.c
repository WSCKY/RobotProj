#include "MAX_M8Q.h"

static GPS_DATA_RAW rawGPS_Data = {0};

static UBXStrcutureDef ubxMsg_tx = {UBX_SYN_STX1, UBX_SYN_STX2};
static UBXStrcutureDef ubxMsg_rx = {UBX_SYN_STX1, UBX_SYN_STX2};
#define UBX_TX_PAYLOAD                 48
static uint8_t sendBuffer[UBX_TX_PAYLOAD] = {0};
#define UBX_RX_PAYLOAD                 400
static uint8_t recvBuffer[UBX_RX_PAYLOAD] = {0};

#if FREERTOS_ENABLED
static QueueHandle_t ubx_queue = NULL;
static void ubx_queue_create(void);
#else

#endif /* FREERTOS_ENABLED */

/* ubx decoder configure */
static void reset_decoder(void);

#define CONFIG_MAX_TRAIL               (5)
/* interface configuration: hardware_port(uart1@115200,8,N,1); software_protocol(ubx) */
/* Port Configuration for UART. (set UART mode/UART baudrate, UBX Protocol) */
#define UBX_IF_CONFIG_MSG              {0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00}
#define UBX_IF_CONFIG_MSG_LEN          (20)
static UBX_ERROR ubx_if_config_flag = UBX_UNCONFIG;
static UBX_ERROR ubx_if_config(UBXStrcutureDef *pMsg);
static void ubx_if_config_packet_prepare(UBXStrcutureDef *pMsg);
static const uint8_t ubx_if_config_msg[UBX_IF_CONFIG_MSG_LEN] = UBX_IF_CONFIG_MSG;

/* data rate configuration: measure rate(10); navigation rate(1); time system measurements aligned(0:UTC time); */
static UBX_ERROR ubx_rate_config_flag = UBX_UNCONFIG;
static UBX_ERROR ubx_rate_config(UBXStrcutureDef *pMsg, uint8_t measRate, uint8_t navRate, uint8_t timeRef);

/* navigation engine settings: dynamic platform model(airborne with <2g acceleration) */
#define UBX_NAV_ENG_CONFIG_MSG         {0xFF, 0xFF, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x05, 0x00, 0xfa, 0x00, 0xfa, 0x00, \
                                        0x64, 0x00, 0x2c, 0x01, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define UBX_NAV_ENG_CONFIG_MSG_LEN     (36)
static UBX_ERROR ubx_nav_eng_config_flag = UBX_UNCONFIG;
static UBX_ERROR ubx_nav_eng_config(UBXStrcutureDef *pMsg, DYNC_PLAT_MODEL dynModel);
static void ubx_nav_eng_config_packet_prepare(UBXStrcutureDef *pMsg, DYNC_PLAT_MODEL dynModel);
static const uint8_t ubx_nav_eng_config_msg[UBX_NAV_ENG_CONFIG_MSG_LEN] = UBX_NAV_ENG_CONFIG_MSG;

/* meassage rate configuration: msg class(0x01); msg id(0x07); msg rate(10) */
static UBX_ERROR ubx_msg_config_flag = UBX_UNCONFIG;
static UBX_ERROR ubx_msg_config(UBXStrcutureDef *pMsg, uint8_t msgCLASS, uint8_t msgID, uint8_t msgRATE);

/* Navigation Position Velocity Time Solution data process */
static void ubx_nav_pvt_msg_process(UBXStrcutureDef *pMsg);

static void ubx_send(UBXStrcutureDef *pMsg);
static void ubx_checksum(UBXStrcutureDef *pMsg);
static uint32_t ubx_packet_verify(UBXStrcutureDef *pMsg);

/**
  * @brief  Initialize MAX-M8Q Module.
  * @param  None
  * @retval None
  */
UBX_ERROR ublox_m8q_init(void)
{
  /* init uart */
  ubx_port_init();

  ubxMsg_tx.pPayload = sendBuffer;
  ubxMsg_rx.pPayload = recvBuffer;
#if FREERTOS_ENABLED
  ubx_queue_create();
#endif /* FREERTOS_ENABLED */
  /* configure hardware & software if */
  if(ubx_if_config(&ubxMsg_tx) != UBX_OK) return UBX_FAILED;
  if(ubx_rate_config(&ubxMsg_tx, 10, 1, 0) != UBX_OK) return UBX_FAILED;
  if(ubx_nav_eng_config(&ubxMsg_tx, lessThan2G) != UBX_OK) return UBX_FAILED;
  if(ubx_msg_config(&ubxMsg_tx, UBX_CLASS_NAV, UBX_ID_NAV_PVT, 10) != UBX_OK) return UBX_FAILED;
  return UBX_OK;
}

#if FREERTOS_ENABLED
static void ubx_queue_create(void)
{
  ubx_queue = xQueueCreate(1, sizeof(GPS_DATA_RAW));
}
#endif /* FREERTOS_ENABLED */

/* --------------------------- UBX H/S IF CONFIG --------------------------- */
static void ubx_if_config_packet_prepare(UBXStrcutureDef *pMsg)
{
  uint8_t i = 0;
  pMsg->CLASS = UBX_CLASS_CFG;
  pMsg->ID    = UBX_ID_CFG_PRT;
  pMsg->LEN_L = UBX_IF_CONFIG_MSG_LEN & 0xFF;
  pMsg->LEN_H = (UBX_IF_CONFIG_MSG_LEN >> 8) & 0xFF;

  for(i = 0; i < UBX_IF_CONFIG_MSG_LEN; i ++) {
    pMsg->pPayload[i] = ubx_if_config_msg[i];
  }
  ubx_checksum(pMsg);
}
static UBX_ERROR ubx_if_config(UBXStrcutureDef *pMsg)
{
  uint8_t MaxTrail = CONFIG_MAX_TRAIL;
  ubx_if_config_packet_prepare(&ubxMsg_tx);
  while(MaxTrail --) {
    ubx_port_config(9600, RX_DISABLE);
    ubx_delay(1);
    reset_decoder();
    ubx_send(&ubxMsg_tx);
    ubx_delay(2); /* wait for the MAX-M8Q Module Ready */
    ubx_port_config(115200, RX_ENABLE); /* start receive */
    ubx_delay(1);
    ubx_send(&ubxMsg_tx); /* send a test command to get a response. */
    ubx_delay(2); /* wait for the MAX-M8Q Module Ack */
    if(ubx_if_config_flag == UBX_CONFIGED) {
      return UBX_OK;
    } else {
      ubx_send(&ubxMsg_tx); /* send a test command to get a response. */
      ubx_delay(2); /* wait for the MAX-M8Q Module Ack */
      if(ubx_if_config_flag == UBX_CONFIGED) {
        return UBX_OK;
      }
    }
  }
//  ubx_port_config(9600, RX_ENABLE);
  return UBX_FAILED;
}

/* --------------------------- UBX RATE CONFIG --------------------------- */
static UBX_ERROR ubx_rate_config(UBXStrcutureDef *pMsg, uint8_t measRate, uint8_t navRate, uint8_t timeRef)
{
  uint8_t MaxTrail = CONFIG_MAX_TRAIL;
  uint16_t elapsed_time = 1000 / measRate;
  pMsg->CLASS = UBX_CLASS_CFG;
  pMsg->ID = UBX_ID_CFG_RATE;
  pMsg->LEN_L = 0x06; pMsg->LEN_H = 0x00;
  pMsg->pPayload[0] = elapsed_time & 0x00FF;
  pMsg->pPayload[1] = (elapsed_time >> 8) & 0xFF;
  pMsg->pPayload[2] = navRate & 0xFF;
  pMsg->pPayload[3] = (navRate >> 8) & 0xFF;
  pMsg->pPayload[4] = timeRef & 0xFF;
  pMsg->pPayload[5] = (timeRef >> 8) & 0xFF;
  ubx_checksum(pMsg);
  while(MaxTrail --) {
    ubx_send(pMsg);
    ubx_delay(2); /* wait for the MAX-M8Q Module Ack */
    if(ubx_rate_config_flag == UBX_CONFIGED) {
      return UBX_OK;
    }
  }
  return UBX_FAILED;
}

/* --------------------------- UBX NAV_ENG CONFIG --------------------------- */
static void ubx_nav_eng_config_packet_prepare(UBXStrcutureDef *pMsg, DYNC_PLAT_MODEL dynModel)
{
  uint8_t i = 0;
  pMsg->CLASS = UBX_CLASS_CFG;
  pMsg->ID    = UBX_ID_CFG_NAV5;
  pMsg->LEN_L = UBX_NAV_ENG_CONFIG_MSG_LEN & 0xFF;
  pMsg->LEN_H = (UBX_NAV_ENG_CONFIG_MSG_LEN >> 8) & 0xFF;

  for(i = 0; i < UBX_NAV_ENG_CONFIG_MSG_LEN; i ++) {
    pMsg->pPayload[i] = ubx_nav_eng_config_msg[i];
  }
  pMsg->pPayload[2] = dynModel;
  ubx_checksum(pMsg);
}
static UBX_ERROR ubx_nav_eng_config(UBXStrcutureDef *pMsg, DYNC_PLAT_MODEL dynModel)
{
  uint8_t MaxTrail = CONFIG_MAX_TRAIL;
  ubx_nav_eng_config_packet_prepare(pMsg, dynModel);
  while(MaxTrail --) {
    ubx_send(pMsg);
    ubx_delay(2); /* wait for the MAX-M8Q Module Ack */
    if(ubx_nav_eng_config_flag == UBX_CONFIGED) {
      return UBX_OK;
    }
  }
  return UBX_FAILED;
}

/* --------------------------- UBX MSG CONFIG --------------------------- */
static UBX_ERROR ubx_msg_config(UBXStrcutureDef *pMsg, uint8_t msgCLASS, uint8_t msgID, uint8_t msgRATE)
{
  uint8_t MaxTrail = CONFIG_MAX_TRAIL;
  pMsg->CLASS = UBX_CLASS_CFG;
  pMsg->ID = UBX_ID_CFG_MSG;
  pMsg->LEN_L = 0x03; pMsg->LEN_H = 0x00;
  pMsg->pPayload[0] = msgCLASS;
  pMsg->pPayload[1] = msgID;
  pMsg->pPayload[2] = msgRATE;
  ubx_checksum(pMsg);
  while(MaxTrail --) {
    ubx_send(pMsg);
    ubx_delay(2); /* wait for the MAX-M8Q Module Ack */
    if(ubx_msg_config_flag == UBX_CONFIGED) {
      return UBX_OK;
    }
  }
  return UBX_FAILED;
}

static void ubx_nav_pvt_msg_process(UBXStrcutureDef *pMsg)
{
#if FREERTOS_ENABLED
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
  uint8_t *p = pMsg->pPayload;
  int32_t tmp = 0;
  uint32_t utmp = 0;
  rawGPS_Data.numSV = p[23];

  tmp = p[27]; tmp <<= 8;
  tmp |= p[26]; tmp <<= 8;
  tmp |= p[25]; tmp <<= 8;
  tmp |= p[24];
  rawGPS_Data.lon = tmp;

  tmp = p[31]; tmp <<= 8;
  tmp |= p[30]; tmp <<= 8;
  tmp |= p[29]; tmp <<= 8;
  tmp |= p[28];
  rawGPS_Data.lat = tmp;

  tmp = p[35]; tmp <<= 8;
  tmp |= p[34]; tmp <<= 8;
  tmp |= p[33]; tmp <<= 8;
  tmp |= p[32];
  rawGPS_Data.height = tmp;

  tmp = p[39]; tmp <<= 8;
  tmp |= p[38]; tmp <<= 8;
  tmp |= p[37]; tmp <<= 8;
  tmp |= p[36];
  rawGPS_Data.hMSL = tmp;

  tmp = p[5]; tmp <<= 8;
  tmp |= p[4];
  rawGPS_Data.year = tmp;
  rawGPS_Data.month = p[6];
  rawGPS_Data.day = p[7];
  rawGPS_Data.hour = p[8];
  rawGPS_Data.min = p[9];
  rawGPS_Data.sec = p[10];
  rawGPS_Data.fixType = p[20];

  utmp = p[43]; utmp <<= 8;
  utmp |= p[42]; utmp <<= 8;
  utmp |= p[41]; utmp <<= 8;
  utmp |= p[40];
  rawGPS_Data.hAcc = utmp;

  utmp = p[47]; utmp <<= 8;
  utmp |= p[46]; utmp <<= 8;
  utmp |= p[45]; utmp <<= 8;
  utmp |= p[44];
  rawGPS_Data.vAcc = utmp;

  tmp = p[51]; tmp <<= 8;
  tmp |= p[50]; tmp <<= 8;
  tmp |= p[49]; tmp <<= 8;
  tmp |= p[48];
  rawGPS_Data.velN = tmp;

  tmp = p[55]; tmp <<= 8;
  tmp |= p[54]; tmp <<= 8;
  tmp |= p[53]; tmp <<= 8;
  tmp |= p[52];
  rawGPS_Data.velE = tmp;

  tmp = p[59]; tmp <<= 8;
  tmp |= p[58]; tmp <<= 8;
  tmp |= p[57]; tmp <<= 8;
  tmp |= p[56];
  rawGPS_Data.velD = tmp;

  tmp = p[87]; tmp <<= 8;
  tmp |= p[86]; tmp <<= 8;
  tmp |= p[85]; tmp <<= 8;
  tmp |= p[84];
  rawGPS_Data.headVeh = tmp;

  utmp = p[71]; utmp <<= 8;
  utmp |= p[70]; utmp <<= 8;
  utmp |= p[69]; utmp <<= 8;
  utmp |= p[68];
  rawGPS_Data.sAcc = utmp;
#if FREERTOS_ENABLED
        xQueueSendFromISR(ubx_queue, &rawGPS_Data, &xHigherPriorityTaskWoken);
#else
        
#endif /* FREERTOS_ENABLED */
}

static void ubx_checksum(UBXStrcutureDef *pMsg)
{
  uint16_t i = 0, len = (pMsg->LEN_H << 8) + (pMsg->LEN_L);
  uint8_t *p = (uint8_t *)&(pMsg->CLASS);
  pMsg->CK_A = 0;
  pMsg->CK_B = 0;
  for(i = 0; i < 4; i ++) {
    pMsg->CK_A += p[i];
    pMsg->CK_B += pMsg->CK_A;
  }
  p = pMsg->pPayload;
  for(i = 0; i < len; i ++) {
    pMsg->CK_A += p[i];
    pMsg->CK_B += pMsg->CK_A;
  }
}

static uint32_t ubx_packet_verify(UBXStrcutureDef *pMsg)
{
  uint16_t i = 0, len = (pMsg->LEN_H << 8) + (pMsg->LEN_L);
  uint8_t ch_a = 0, ch_b = 0;
  uint8_t *p = (uint8_t *)&(pMsg->CLASS);
  for(i = 0; i < 4; i ++) {
    ch_a += p[i];
    ch_b += ch_a;
  }
  p = pMsg->pPayload;
  for(i = 0; i < len; i ++) {
    ch_a += p[i];
    ch_b += ch_a;
  }
  if((ch_a == pMsg->CK_A) && (ch_b == pMsg->CK_B)) {
    return 1;
  }
  return 0;
}

static void ubx_send(UBXStrcutureDef *pMsg)
{
  uint32_t l = (pMsg->LEN_H << 8) + (pMsg->LEN_L);
  ubx_send_bytes((uint8_t *)pMsg, 6);
  ubx_send_bytes(pMsg->pPayload, l);
  ubx_send_bytes((uint8_t *)&(pMsg->CK_A), 2);
}

static uint16_t _rx_payload_cnt = 0;
static uint16_t _rx_payload_len = 0;
static UBX_DECODE_STATE _decode_state = UBX_DECODE_UNSYNCED;
static void reset_decoder(void) { _decode_state = UBX_DECODE_UNSYNCED; }

void ubxPortRxDataCallback(uint8_t Data)
{
  switch(_decode_state) {
    case UBX_DECODE_UNSYNCED:
      if(Data == UBX_SYN_STX1) {
        ubxMsg_rx.SYNCHAR1 = Data;
        _decode_state = UBX_DECODE_GOT_SYN1;
      }
    break;
    case UBX_DECODE_GOT_SYN1:
      if(Data == UBX_SYN_STX2) {
        ubxMsg_rx.SYNCHAR2 = Data;
        _decode_state = UBX_DECODE_GOT_SYN2;
      } else {
        _decode_state = UBX_DECODE_UNSYNCED;
      }
    break;
    case UBX_DECODE_GOT_SYN2:
      ubxMsg_rx.CLASS = Data;
      _decode_state = UBX_DECODE_GOT_CLASS;
    break;
    case UBX_DECODE_GOT_CLASS:
      ubxMsg_rx.ID = Data;
      _decode_state = UBX_DECODE_GOT_ID;
    break;
    case UBX_DECODE_GOT_ID:
      ubxMsg_rx.LEN_L = Data;
      _rx_payload_len = Data;
      _decode_state = UBX_DECODE_GOT_LEN_L;
    break;
    case UBX_DECODE_GOT_LEN_L:
      ubxMsg_rx.LEN_H = Data;
      _rx_payload_len += ((uint16_t)Data << 8);
      if(_rx_payload_len > UBX_RX_PAYLOAD) {
        _rx_payload_len = 0;
        _decode_state = UBX_DECODE_UNSYNCED;
      } else {
        _rx_payload_cnt = 0;
        _decode_state = UBX_DECODE_GOT_LEN_H;
      }
    break;
    case UBX_DECODE_GOT_LEN_H:
      ubxMsg_rx.pPayload[_rx_payload_cnt ++] = Data;
      if(_rx_payload_cnt >= _rx_payload_len) {
        _rx_payload_cnt = 0;
        _decode_state = UBX_DECODE_GOT_DATA;
      }
    break;
    case UBX_DECODE_GOT_DATA:
      ubxMsg_rx.CK_A = Data;
      _decode_state = UBX_DECODE_GOT_CK_A;
    break;
    case UBX_DECODE_GOT_CK_A:
      ubxMsg_rx.CK_B = Data;
      _decode_state = UBX_DECODE_UNSYNCED;
      if(ubx_packet_verify(&ubxMsg_rx)) {
        if(ubx_if_config_flag != UBX_CONFIGED) {
          if((ubxMsg_rx.CLASS == UBX_CLASS_ACK) && (ubxMsg_rx.ID == UBX_ID_ACK_ACK) && (ubxMsg_rx.pPayload[0] == UBX_CLASS_CFG) && (ubxMsg_rx.pPayload[1] == UBX_ID_CFG_PRT)) {
            ubx_if_config_flag = UBX_CONFIGED;
          }
        } else if(ubx_rate_config_flag != UBX_CONFIGED) {
          if((ubxMsg_rx.CLASS == UBX_CLASS_ACK) && (ubxMsg_rx.ID == UBX_ID_ACK_ACK) && (ubxMsg_rx.pPayload[0] == UBX_CLASS_CFG) && (ubxMsg_rx.pPayload[1] == UBX_ID_CFG_RATE)) {
            ubx_rate_config_flag = UBX_CONFIGED;
          }
        } else if(ubx_nav_eng_config_flag != UBX_CONFIGED) {
          if((ubxMsg_rx.CLASS == UBX_CLASS_ACK) && (ubxMsg_rx.ID == UBX_ID_ACK_ACK) && (ubxMsg_rx.pPayload[0] == UBX_CLASS_CFG) && (ubxMsg_rx.pPayload[1] == UBX_ID_CFG_NAV5)) {
            ubx_nav_eng_config_flag = UBX_CONFIGED;
          }
        } else if(ubx_msg_config_flag != UBX_CONFIGED) {
          if((ubxMsg_rx.CLASS == UBX_CLASS_ACK) && (ubxMsg_rx.ID == UBX_ID_ACK_ACK) && (ubxMsg_rx.pPayload[0] == UBX_CLASS_CFG) && (ubxMsg_rx.pPayload[1] == UBX_ID_CFG_MSG)) {
            ubx_msg_config_flag = UBX_CONFIGED;
          }
        } else {
          if(ubxMsg_rx.CLASS == UBX_CLASS_NAV && ubxMsg_rx.ID == UBX_ID_NAV_PVT) {
            ubx_nav_pvt_msg_process(&ubxMsg_rx);
          }
        }
      }
    break;
    default:
      _decode_state = UBX_DECODE_UNSYNCED;
    break;
  }
}
