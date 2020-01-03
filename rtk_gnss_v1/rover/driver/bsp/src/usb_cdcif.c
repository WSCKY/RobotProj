/**
  ******************************************************************************
  * @file    driver/bsp/src/usbd_cdcif.c
  * @author  kyChu<kychu@qq.com>
  * @brief   Source file for USBD CDC interface driver.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_cdcif.h"
#include "ringbuffer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CDC_DRV_RX_DATA_SIZE  512
#define CDC_RX_RB_CACHE_SIZE  1024
//#define APP_TX_DATA_SIZE  1024

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static USBD_HandleTypeDef *usb_dev_cdc;

static ringbuffer_handle usb_cdc_rx_rb;
static uint8_t *usb_cdc_rx_rb_cache = NULL;

static osSemaphoreId cdc_rx_abailable_sync = NULL;

//__ALIGN_BEGIN uint8_t UserRxBuffer[APP_RX_DATA_SIZE] __ALIGN_END;/* Received Data over USB are stored in this buffer */
uint8_t *pUserRxBuffer = NULL;
//__ALIGN_BEGIN uint8_t UserTxBuffer[APP_TX_DATA_SIZE] __ALIGN_END;/* Received Data over UART (CDC interface) are stored in this buffer */
uint32_t BuffLength;
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */

/* Private function prototypes -----------------------------------------------*/
static int8_t CDC_Itf_Init(void);
static int8_t CDC_Itf_DeInit(void);
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Itf_Receive(uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_CDC_fops = 
{
  CDC_Itf_Init,
  CDC_Itf_DeInit,
  CDC_Itf_Control,
  CDC_Itf_Receive
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  CDC_Itf_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Init(void)
{
  /*##-5- Set Application Buffers ############################################*/
//  USBD_CDC_SetTxBuffer(&USBD_Device, UserTxBuffer, 0);
  USBD_CDC_SetRxBuffer(usb_dev_cdc, pUserRxBuffer);
  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_DeInit(void)
{
  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Control(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
}

/**
  * @brief  CDC_Itf_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  * @param  Buf: Buffer of data to be transmitted
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Itf_Receive(uint8_t* Buf, uint32_t *Len)
{
  ringbuffer_push(&usb_cdc_rx_rb, Buf, *Len);
  osSemaphoreRelease(cdc_rx_abailable_sync);

  USBD_CDC_SetRxBuffer(usb_dev_cdc, &Buf[0]);
  USBD_CDC_ReceivePacket(usb_dev_cdc);
  return (USBD_OK);
}

/* Public functions ----------------------------------------------------------*/

status_t cdcif_init(void)
{
  usb_dev_cdc = kmm_alloc(sizeof(USBD_HandleTypeDef));
  if(usb_dev_cdc == NULL) return status_nomem;

  /* Init Device Library */
  if(USBD_Init(usb_dev_cdc, &VCP_Desc, 0) != USBD_OK) return status_error;

  /* Add Supported Class */
  if(USBD_RegisterClass(usb_dev_cdc, USBD_CDC_CLASS) != USBD_OK) return status_error;

  /* Add CDC Interface Class */
  if(USBD_CDC_RegisterInterface(usb_dev_cdc, &USBD_CDC_fops) != USBD_OK) return status_error;
  /* alloc memory for CDC class */
  if(USBD_CDC_MallocClassData(usb_dev_cdc) != USBD_OK) return status_nomem;
  /* alloc memory for CDC receive cache */
  pUserRxBuffer = kmm_alloc(CDC_DRV_RX_DATA_SIZE);
  if(pUserRxBuffer == NULL) return status_nomem;

  usb_cdc_rx_rb_cache = kmm_alloc(CDC_RX_RB_CACHE_SIZE);
  if(usb_cdc_rx_rb_cache == NULL) return status_nomem;

  ringbuffer_init(&usb_cdc_rx_rb, usb_cdc_rx_rb_cache, CDC_RX_RB_CACHE_SIZE);

  osSemaphoreDef(CDC_RX_SYNC);
  cdc_rx_abailable_sync = osSemaphoreCreate(osSemaphore(CDC_RX_SYNC), 1);
  if(cdc_rx_abailable_sync == NULL) return status_nomem;

  /* Start Device Process */
  USBD_Start(usb_dev_cdc);

  return status_ok;
}

status_t cdcif_tx_bytes(uint8_t *p, uint32_t l)
{
  USBD_CDC_SetTxBuffer(usb_dev_cdc, p, l);

  if(USBD_CDC_TransmitPacket(usb_dev_cdc) == USBD_OK) return status_ok;
  return status_error;
}

uint32_t cdcif_rx_bytes(uint8_t *p, uint32_t l, uint32_t timeout)
{
  if(osSemaphoreWait(cdc_rx_abailable_sync, timeout) == osOK)
    return ringbuffer_poll(&usb_cdc_rx_rb, p, l);
  return 0;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
