#include "mesg.h"
#include "kyLink.h"
#include "gnss.h"
#include <math.h>

static KYLINK_CORE_HANDLE *kylink_msg;

static uint8_t *mesg_data_cache;
static uint8_t *kylink_decoder_cache;

static uint8_t *mesg_test_send_cache;

#define KYLINK_MSG_CACHE_SIZE                    (128)

/* Semaphore to signal incoming packets */
static QueueHandle_t com_msg_q = NULL;

static void mesg_decode_task(void const *argument);
static void mesg_decode_callback(kyLinkBlockDef *pRx);

static uint32_t msg_data_length = 0;
static status_t test_mesg_cache_send(uint8_t *p, uint32_t l);

void mesg_send_task(void const *argument)
{
  COM_MSG_DEF *msg = NULL;
  kyLinkConfig_t *cfg = NULL;

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
  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  kylink_msg = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  mesg_data_cache = kmm_alloc(KYLINK_MSG_CACHE_SIZE);
  kylink_decoder_cache = kmm_alloc(KYLINK_MSG_CACHE_SIZE);

  mesg_test_send_cache = kmm_alloc(KYLINK_MSG_CACHE_SIZE + 8);

  if(msg == NULL || cfg == NULL || kylink_msg == NULL || mesg_data_cache == NULL || kylink_decoder_cache == NULL || mesg_test_send_cache == NULL) {
    kmm_free(msg);
    kmm_free(cfg);
    kmm_free(kylink_msg);
    kmm_free(mesg_data_cache);
    kmm_free(kylink_decoder_cache);
    kmm_free(mesg_test_send_cache);
    ky_err("mesg memory alloc failed, EXIT!\n");
    vTaskDelete(NULL);
  }

  cfg->txfunc = test_mesg_cache_send;//cdcif_tx_bytes;
  cfg->callback = mesg_decode_callback;
  cfg->cache_size = KYLINK_MSG_CACHE_SIZE;
  cfg->decoder_cache = kylink_decoder_cache;
  kylink_init(kylink_msg, cfg);

  kmm_free(cfg);

  ky_info("mesg module started.\n");

  osThreadDef(DECODE, mesg_decode_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2); // stack size = 256B
  if(osThreadCreate(osThread(DECODE), NULL) == NULL) ky_err("mesg decode task create failed.\n");

  for(;;) {
    if(xQueueReceive(com_msg_q, msg, osWaitForever) == pdPASS) {
      ( void ) memcpy( ( void * ) mesg_data_cache, ( void * ) msg->pointer, ( size_t ) msg->len );
      // indicate read operation.
      *((msg_wr_state *)(msg->state)) = msg_read;
      kylink_send(kylink_msg, ( void * ) mesg_data_cache, msg->type, msg->len);
      if(msg_data_length > 0) {
        cdcif_tx_bytes(mesg_test_send_cache, msg_data_length);
        msg_data_length = 0;
      }
    }
  }
}

static status_t test_mesg_cache_send(uint8_t *p, uint32_t l)
{
  if((msg_data_length + l) < (KYLINK_MSG_CACHE_SIZE + 8)) {
    memcpy((void *)mesg_test_send_cache + msg_data_length, (void *)p, (size_t)l);
    msg_data_length += l;
  }
  return status_ok;
}

void mesg_send_mesg(void *msg)
{
  if(com_msg_q == NULL) return;
  xQueueSend(com_msg_q, msg, osWaitForever);
}

static void mesg_decode_callback(kyLinkBlockDef *pRx)
{
  ky_info("recv new mesg: 0x%x, 0x%x.\n", pRx->msg_id, pRx->dev_id);
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
  ky_info("mesg decoder start.\n");
  for(;;) {
    rx_len = cdcif_rx_bytes(pRxCache, 128, osWaitForever);
    decode_cnt = 0;
    while(decode_cnt < rx_len) {
      kylink_decode(kylink_msg, pRxCache[decode_cnt ++]);
    }
  }
}
