#include "com_task.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

static COM_MSG_DEF msg;
static QueueHandle_t com_msg_q = NULL;

static CommPackageDef TxPacket;// = {.stx1 = kySTX1, .stx2 = kySTX2};
static CommPackageDef RxPacket;// = {.stx1 = kySTX1, .stx2 = kySTX2};

static void COM_TX_Thread(void const *argument);
static void COM_RX_Thread(void const *argument);

static void kyLinkByteHandler(uint8_t Data);
static void kyLinkBytesHandler(uint8_t *p, uint32_t l);

void COM_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);

  /* hardware initialize. */
  ComPort_Init();
  kyLinkByteHandler(0); // warning ...
  USB_CDC_CallbackRegistry(kyLinkBytesHandler); /* register callback handler function */
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

  kyLink_Init();

  osThreadDef(COM_TX, COM_TX_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 4);
  osThreadCreate(osThread(COM_TX), NULL);
  osThreadDef(COM_RX, COM_RX_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
  osThreadCreate(osThread(COM_RX), NULL);

  vTaskDelete(NULL);
}

static void COM_TX_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  uint32_t index = 0;
  portBASE_TYPE xTaskWokenByReceive = pdFALSE;

  com_msg_q = xQueueCreate(10, sizeof(COM_MSG_DEF));
  TxPacket.Packet.stx1 = kySTX1;
  TxPacket.Packet.stx2 = kySTX2;
  TxPacket.Packet.dev_id = HARD_DEV_ID;
  TxPacket.Packet.msg_id = TYPE_IMU_INFO_Resp;
  TxPacket.Packet.length = sizeof(IMU_INFO_DEF);
  for(;;) {
    if(xQueueReceiveFromISR(com_msg_q, &msg, &xTaskWokenByReceive) == pdPASS) {
      TxPacket.Packet.length = msg.len;
      TxPacket.Packet.msg_id = msg.type;
      for(index = 0; index < msg.len; index ++) {
        TxPacket.Packet.PacketData.pData[index] = ((uint8_t *)msg.pointer)[index];
      }
      SendTxPacket(&TxPacket);
    }
  }
}

static void COM_RX_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  portBASE_TYPE xTaskWokenByReceive = pdFALSE; 
  for(;;) {
    if(xQueueReceiveFromISR(lnk_recv_q, &RxPacket, &xTaskWokenByReceive) == pdPASS) {

    }
  }
}

QueueHandle_t* get_com_msg_send_queue(void)
{
  return &com_msg_q;
}

static void kyLinkBytesHandler(uint8_t *p, uint32_t l)
{
  while(l --) {
    kyLink_DecodeProcess(*p ++);
  }
}

static void kyLinkByteHandler(uint8_t Data)
{
//  kyLink_DecodeProcess(Data);
}
