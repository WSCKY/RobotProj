/**
  ******************************************************************************
  * @file    USB_Device/CDC_Standalone/Src/usbd_cdc_if.c
  * @author  kyChu<kychu@qq.com>
  * @brief   Source file for USBD CDC interface
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usb_cdcif.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define APP_RX_DATA_SIZE  1024
//#define APP_TX_DATA_SIZE  1024

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
USBD_HandleTypeDef USBD_Device;

USBD_CDC_LineCodingTypeDef LineCoding =
{
  115200, /* baud rate*/
  0x00,   /* stop bits-1*/
  0x00,   /* parity - none*/
  0x08    /* nb. of bits 8*/
};

//__ALIGN_BEGIN uint8_t UserRxBuffer[APP_RX_DATA_SIZE] __ALIGN_END;/* Received Data over USB are stored in this buffer */
uint8_t *pUserRxBuffer = NULL;
//__ALIGN_BEGIN uint8_t UserTxBuffer[APP_TX_DATA_SIZE] __ALIGN_END;/* Received Data over UART (CDC interface) are stored in this buffer */
uint32_t BuffLength;
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */

/* USB handler declaration */
extern USBD_HandleTypeDef  USBD_Device;

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
  USBD_CDC_SetRxBuffer(&USBD_Device, pUserRxBuffer);
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
  USBD_CDC_SetRxBuffer(&USBD_Device, &Buf[0]);
  USBD_CDC_ReceivePacket(&USBD_Device);
  return (USBD_OK);
}

/* Public functions ----------------------------------------------------------*/

status_t cdcif_init(void)
{
  /* Init Device Library */
  if(USBD_Init(&USBD_Device, &VCP_Desc, 0) != USBD_OK) return status_error;

  /* Add Supported Class */
  if(USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS) != USBD_OK) return status_error;

  /* Add CDC Interface Class */
  if(USBD_CDC_RegisterInterface(&USBD_Device, &USBD_CDC_fops) != USBD_OK) return status_error;
  /* alloc memory for CDC class */
  if(USBD_CDC_MallocClassData(&USBD_Device) != USBD_OK) return status_nomem;
  /* alloc memory for CDC receive cache */
  pUserRxBuffer = kmm_alloc(APP_RX_DATA_SIZE);
  if(pUserRxBuffer == NULL) return status_nomem;

  /* Start Device Process */
  USBD_Start(&USBD_Device);

  return status_ok;
}

status_t cdcif_tx_bytes(uint8_t *p, uint32_t l)
{
  USBD_CDC_SetTxBuffer(&USBD_Device, p, l);

  if(USBD_CDC_TransmitPacket(&USBD_Device) == USBD_OK) return status_ok;
  return status_error;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/