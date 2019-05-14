#include "com_task.h"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

static COM_MSG_DEF msg;
static QueueHandle_t com_msg_q = NULL;

KYLINK_CORE_HANDLE CDC_PortHandle;

static kyLinkPackageDef TxPacket;// = {.stx1 = kySTX1, .stx2 = kySTX2};
//static CommPackageDef RxPacket;// = {.stx1 = kySTX1, .stx2 = kySTX2};

static void COM_TX_Thread(void const *argument);
//static void COM_RX_Thread(void const *argument);

static void CDC_RecvBytesHandler(uint8_t *p, uint32_t l);

void COM_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);

  /* hardware initialize. */
  com_port_init();
  USB_CDC_CallbackRegistry(CDC_RecvBytesHandler); /* register callback handler function */
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

  kyLinkInit(&CDC_PortHandle, USB_CDC_SendBufferFast);

  osThreadDef(COM_TX, COM_TX_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 4);
  osThreadCreate(osThread(COM_TX), NULL);
//  osThreadDef(COM_RX, COM_RX_Thread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);
//  osThreadCreate(osThread(COM_RX), NULL);

  vTaskDelete(NULL);
}

static void COM_TX_Thread(void const *argument)
{
  UNUSED_PARAMETER(argument);
  uint32_t index = 0;
  portBASE_TYPE xTaskWokenByReceive = pdFALSE;

  com_msg_q = xQueueCreate(10, sizeof(COM_MSG_DEF));
  TxPacket.FormatData.stx1 = kySTX1;
  TxPacket.FormatData.stx2 = kySTX2;
  TxPacket.FormatData.dev_id = HARD_DEV_ID;
  TxPacket.FormatData.msg_id = TYPE_IMU_6DOF_Resp;
  TxPacket.FormatData.length = sizeof(IMU_6DOF_DEF);
  for(;;) {
    if(xQueueReceiveFromISR(com_msg_q, &msg, &xTaskWokenByReceive) == pdPASS) {
      TxPacket.FormatData.length = msg.len;
      TxPacket.FormatData.msg_id = msg.type;
      for(index = 0; index < msg.len; index ++) {
        TxPacket.FormatData.PacketData.RawData[index] = ((uint8_t *)msg.pointer)[index];
      }
      SendTxPacket(&CDC_PortHandle, &TxPacket);
    }
  }
}

//static void COM_RX_Thread(void const *argument)
//{
//  UNUSED_PARAMETER(argument);
//  portBASE_TYPE xTaskWokenByReceive = pdFALSE;
//  for(;;) {
//    if(xQueueReceiveFromISR(lnk_recv_q, &RxPacket, &xTaskWokenByReceive) == pdPASS) {
//
//    }
//  }
//}

QueueHandle_t* get_com_msg_send_queue(void)
{
  return &com_msg_q;
}

void USBD_USR_DeviceConfigured(void)
{
	kyLinkTxEnable(&CDC_PortHandle);
}

void USBD_USR_DeviceSuspended(void)
{
	kyLinkTxDisable(&CDC_PortHandle);
}

static void CDC_RecvBytesHandler(uint8_t *p, uint32_t l)
{
  while(l --) {
	  kylink_decode(&CDC_PortHandle, *p ++);
  }
}
