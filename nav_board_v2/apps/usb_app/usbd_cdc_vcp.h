/**
  ******************************************************************************
  * @file    usbd_cdc_vcp.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-January-2014
  * @brief   Header for usbd_cdc_vcp.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_VCP_H
#define __USBD_CDC_VCP_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_core.h"

/* Exported typef ------------------------------------------------------------*/
/* The following structures groups all needed parameters to be configured for the 
   ComPort. These parameters can be modified on the fly by the host through CDC class
   command class requests. */
typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
}LINE_CODING;

/* Exported constants --------------------------------------------------------*/
#define USB_IT_PRIO                    1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t USB_CDC_SendChar(uint8_t c);
void USB_CDC_CallbackRegistry(PortRecvBytesCallback p);
void USB_CDC_SendBuffer(uint8_t *Buffer, uint32_t Length);
void USB_CDC_SendCharFast(uint8_t c);
void USB_CDC_SendBufferFast(uint8_t *pBuffer, uint32_t Length);

#endif /* __USBD_CDC_VCP_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
