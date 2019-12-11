/**
  ******************************************************************************
  * @file    sysconfig/usbd_conf.h
  * @author  kyChu<kychu@qq.com>
  * @brief   General low level driver configuration
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CONF_H
#define __USBD_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "SysConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Common Config */
#define USBD_MAX_NUM_INTERFACES               1
#define USBD_MAX_NUM_CONFIGURATION            1
#define USBD_MAX_STR_DESC_SIZ                 0xFF
#define USBD_SUPPORT_USER_STRING              0 
#define USBD_SELF_POWERED                     1
#define USBD_DEBUG_LEVEL                      0

/* Exported macro ------------------------------------------------------------*/
/* Memory management macros */   
#define USBD_malloc               kmm_alloc
#define USBD_free                 kmm_free
//#define USBD_memset               memset
//#define USBD_memcpy               memcpy

/* DEBUG macros */
#if (USBD_DEBUG_LEVEL > 0)
#define  USBD_UsrLog(...)   printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 1)

#define  USBD_ErrLog(...)   printf("ERROR: ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif

#if (USBD_DEBUG_LEVEL > 2)
#define  USBD_DbgLog(...)   printf("DEBUG : ") ;\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif

/* Exported functions ------------------------------------------------------- */

#endif /* __USBD_CONF_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
