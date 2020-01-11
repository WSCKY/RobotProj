#include "mesg.h"
#include "kyLink.h"
#include <math.h>

#include "filetransfer.h"

static const char *TAG = "MESG";

static KYLINK_CORE_HANDLE *kylink_msg;

static uint8_t *kylink_decoder_cache;
static uint8_t *mesg_test_send_cache;

#define KYLINK_DECODER_CACHE_SIZE                (128)
#define KYLINK_MSG_CACHE_SIZE                    (768)

static osMutexId msg_mutex = NULL;
/* Semaphore to signal incoming packets */
static osSemaphoreId msg_available = NULL;

static struct MsgList *msg_list = NULL;

static uint32_t _task_running = 0;

static void mesg_decode_task(void const *argument);
static void mesg_decode_callback(kyLinkBlockDef *pRx);

static uint32_t msg_data_length = 0;
static status_t mesg_send_cache(uint8_t *p, uint32_t l);

void mesg_proc_task(void const *argument)
{
  kyLinkConfig_t *cfg = NULL;

  if(cdcif_init() != status_ok) {
    ky_err(TAG, "usb cdc init failed, EXIT!");
    vTaskDelete(NULL);
  }

  osMutexDef(MSG_MUTEX);
  msg_mutex = osMutexCreate(osMutex(MSG_MUTEX));
  if(msg_mutex == NULL) {
    ky_err(TAG, "create MSG MUTEX failed, EXIT!");
    vTaskDelete(NULL);
  }

  osSemaphoreDef(MSG_SEMA);
  msg_available = osSemaphoreCreate(osSemaphore(MSG_SEMA), 1);
  if(msg_available == NULL) {
    ky_err(TAG, "create MSG SEMA failed, EXIT!");
    osMutexDelete(msg_mutex);
    vTaskDelete(NULL);
  }

  cfg = kmm_alloc(sizeof(kyLinkConfig_t));
  kylink_msg = kmm_alloc(sizeof(KYLINK_CORE_HANDLE));
  mesg_test_send_cache = kmm_alloc(KYLINK_MSG_CACHE_SIZE);
  kylink_decoder_cache = kmm_alloc(KYLINK_DECODER_CACHE_SIZE);
  if(cfg == NULL || kylink_msg == NULL || kylink_decoder_cache == NULL || mesg_test_send_cache == NULL) {
    kmm_free(cfg);
    kmm_free(kylink_msg);
    kmm_free(kylink_decoder_cache);
    kmm_free(mesg_test_send_cache);
    osMutexDelete(msg_mutex);
    osSemaphoreDelete(msg_available);
    ky_err(TAG, "MSG memory alloc failed, EXIT!");
    vTaskDelete(NULL);
  }

  cfg->txfunc = mesg_send_cache;
  cfg->callback = mesg_decode_callback;
  cfg->decoder_cache = kylink_decoder_cache;
  cfg->cache_size = KYLINK_DECODER_CACHE_SIZE;
  kylink_init(kylink_msg, cfg);

  kmm_free(cfg);

  _task_running = 1;
  ky_info(TAG, "mesg module start.");

  osThreadDef(DECODE, mesg_decode_task, osPriorityNormal, 0, configMINIMAL_STACK_SIZE); // stack size = 128B
  if(osThreadCreate(osThread(DECODE), NULL) == NULL) ky_err(TAG, "mesg decode task create failed.");

  for(;;) {
    if(osSemaphoreWait(msg_available, osWaitForever) == osOK) {
      osMutexWait(msg_mutex, osWaitForever);
      if(msg_data_length > 0) {
        cdcif_tx_bytes(mesg_test_send_cache, msg_data_length);
        msg_data_length = 0;
      }
      osMutexRelease(msg_mutex);
    }
  }
}

static status_t mesg_send_cache(uint8_t *p, uint32_t l)
{
  if((msg_data_length + l) < (KYLINK_MSG_CACHE_SIZE)) {
    memcpy((void *)mesg_test_send_cache + msg_data_length, (void *)p, (size_t)l);
    msg_data_length += l;
  }
  return status_ok;
}

void mesg_publish_mesg(struct MsgList *list)
{
  struct MsgList *pr = msg_list;
  if(msg_list == NULL)
    msg_list = list;
  else {
    while(pr->next != NULL) {
      pr = pr->next;
    }
    pr->next = list;
  }
}

void mesg_send_mesg(const void *msg, uint8_t msgid, uint16_t len)
{
  if(_task_running == 0) return;
  if(len == 0) return;
  osMutexWait(msg_mutex, osWaitForever);
  kylink_send(kylink_msg, ( void * ) msg, msgid, len);
  osMutexRelease(msg_mutex);
  osSemaphoreRelease(msg_available);
}

static void mesg_decode_callback(kyLinkBlockDef *pRx)
{
//  ky_info("recv new mesg: 0x%x, 0x%x.\n", pRx->msg_id, pRx->dev_id);
  if(pRx->msg_id == MESG_SUBSCRIBE_REQ_MSG) {
    struct MsgList *pr = msg_list;
    while(pr != NULL) {
      if(pr->info->msg_id == pRx->buffer[0]) {
        if(pRx->buffer[2] <= MESG_RATE_MAX) // max rate check
          *(pr->info) = *(struct MsgInfo *)(&pRx->buffer[0]);
        break;
      } else {
        pr = pr->next;
      }
    }
  } else {
    filetransfer_cmd_process(pRx);
  }
}

static void mesg_decode_task(void const *argument)
{
  int decode_cnt, rx_len;
  uint8_t *pRxCache;
  pRxCache = kmm_alloc(128);
  if(pRxCache == NULL) {
    ky_err(TAG, "no memory for mesg decoder task.");
    vTaskDelete(NULL);
  }
  ky_info(TAG, "mesg decoder start.");
  for(;;) {
    rx_len = cdcif_rx_bytes(pRxCache, 128, osWaitForever);
    decode_cnt = 0;
    while(decode_cnt < rx_len) {
      kylink_decode(kylink_msg, pRxCache[decode_cnt ++]);
    }
  }
}
