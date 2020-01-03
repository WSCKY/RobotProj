#include "mesg.h"
#include "kyLink.h"
#include "gnss.h"
#include <math.h>

static KYLINK_CORE_HANDLE *kylink_msg;
static kyLinkPackageDef *tx_packet;

/* Semaphore to signal incoming packets */
static QueueHandle_t com_msg_q = NULL;

static void mesg_decode_task(void const *argument);
static void mesg_decode_callback(kyLinkPackageDef *pRx);

void mesg_send_task(void const *argument)
{
  COM_MSG_DEF *msg = NULL;

  if(cdcif_init() != status_ok) {
    ky_err("usb cdc init failed.\n");
    ky_err("mesg module start failed, EXIT!\n");
    vTaskDelete(NULL);
  }

  com_msg_q = xQueueCreate(10, sizeof(COM_MSG_DEF));
  if(com_msg_q == NULL) {
    ky_err("creates queue instance failed, EXIT!\n");
    vTaskDelete(NULL);
  }

  msg = kmm_alloc(sizeof(COM_MSG_DEF));
  tx_packet = kmm_alloc(sizeof(kyLinkPackageDef));
  kylink_msg = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));

  if(msg == NULL || tx_packet == NULL || kylink_msg == NULL) {
    ky_err("mesg memory alloc failed, EXIT!\n");
    vTaskDelete(NULL);
  }

  ky_info("mesg module started.\n");

  kyLinkInit(kylink_msg);
  kyLinkConfigTxFunc(kylink_msg, cdcif_tx_bytes);
  kyLinkInitPackage(tx_packet);

  osThreadDef(DECODE, mesg_decode_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
  if(osThreadCreate(osThread(DECODE), NULL) == NULL) ky_err("mesg decode task create failed.\n");

  for(;;) {
    if(xQueueReceive(com_msg_q, msg, osWaitForever) == pdPASS) {
      tx_packet->FormatData.length = msg->len;
      tx_packet->FormatData.msg_id = msg->type;
      ( void ) memcpy( ( void * ) &(tx_packet->FormatData.PacketData.RawData[0]), ( void * ) msg->pointer, ( size_t ) msg->len );
      // indicate read operation.
      *((msg_wr_state *)(msg->state)) = msg_read;
      SendTxPacket(kylink_msg, tx_packet);
    }
  }
}

void mesg_send_mesg(void *msg)
{
  if(com_msg_q == NULL) return;
  xQueueSend(com_msg_q, msg, osWaitForever);
}

static void mesg_decode_callback(kyLinkPackageDef *pRx)
{
  ky_info("recv new mesg: 0x%x, 0x%x.\n", pRx->FormatData.msg_id, pRx->FormatData.dev_id);
}

static void mesg_decode_task(void const *argument)
{
  int decode_cnt, rx_len;
  uint8_t *pRxCache;
  pRxCache = kmm_alloc(128);
  if(pRxCache == NULL) {
    ky_err("no memory for mesg decoder task.\n");
    vTaskDelete(NULL);
  }
  kyLinkConfigCbFunc(kylink_msg, mesg_decode_callback);
  ky_info("mesg decoder start.\n");
  for(;;) {
    rx_len = cdcif_rx_bytes(pRxCache, 128, osWaitForever);
    decode_cnt = 0;
    while(decode_cnt < rx_len) {
      kylink_decode(kylink_msg, pRxCache[decode_cnt ++]);
    }
  }
}
