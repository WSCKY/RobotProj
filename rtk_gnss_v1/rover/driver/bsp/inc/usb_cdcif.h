/**
  ******************************************************************************
  * @file    driver/usbdrv/cdc/usbd_cdc_if.h
  * @author  kyChu<kychu@qq.com>
  * @brief   Header for usbd_cdc_if.c file.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CDC_IF_H
#define __USBD_CDC_IF_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"
#include "usbd_core.h"
#include "usbd_desc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

status_t cdcif_init(void);
status_t cdcif_tx_bytes(uint8_t *p, uint32_t l);
uint32_t cdcif_rx_bytes(uint8_t *p, uint32_t l, uint32_t timeout);

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#endif /* __USBD_CDC_IF_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
