/**
  ******************************************************************************
  * @file    ./inc/CAN.h 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   Header for CAN.c module.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CAN_H
#define __CAN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_can.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx_gpio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define CANx                       CAN
#define CAN_CLK                    RCC_APB1Periph_CAN
#define CAN_RX_PIN                 GPIO_Pin_11
#define CAN_TX_PIN                 GPIO_Pin_12
#define CAN_GPIO_PORT              GPIOA
#define CAN_GPIO_CLK               RCC_AHBPeriph_GPIOA
#define CAN_AF_PORT                GPIO_AF_4
#define CAN_RX_SOURCE              GPIO_PinSource11
#define CAN_TX_SOURCE              GPIO_PinSource12

/* Exported macro ------------------------------------------------------------*/
#define CAN_GPIO_REMAP()           do { SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_PA12_RMP; } while(0)
/* Exported functions ------------------------------------------------------- */
void CAN_If_Init(void);
CanRxMsg* GetCAN_RxMsg(void);
uint8_t GetMsgUpdateFlag(void);
void CAN_TransmitData(CanTxMsg* pTxMessage);

#endif /* __CAN_H */

/******************************** END OF FILE *********************************/
