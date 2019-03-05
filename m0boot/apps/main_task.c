/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"
#include "string.h"

//USB_CORE_HANDLE  USB_Device_dev;

CommPackageDef *pRx;
CommPackageDef TxPacket;
uint8_t upgrade_flag = 0;
uint32_t NbrOfPage = 24;

uint32_t PackageNbr = 0, PackageRecNbr = 0;

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;

#define FLASH_PAGE_SIZE         ((uint32_t)0x00000400)   /* FLASH Page Size */
#define APPLICATION_ADDRESS   ((uint32_t)0x08002000)   /* Start @ of user Flash area */
//#define FLASH_USER_END_ADDR     ((uint32_t)0x08008000)   /* End @ of user Flash area */

#define USER_FLASH_PROGRAM_CACHE   (2000)
#define PACKAGE_NUM_PER_CACHE         (USER_FLASH_PROGRAM_CACHE / FILE_DATA_CACHE)

uint8_t PacketDataInCacheIndex = 0;
uint8_t FlashProgramIndex = 0;
uint8_t FlashProgramCache[USER_FLASH_PROGRAM_CACHE] = {0};

uint32_t PacketSendStartTime = 0;
uint32_t PacketSendTimeOut = 0;

uint8_t UpgradeComplete = 0;

FLASH_Status FLASH_If_ProgramWords(uint32_t Address, uint8_t *pData, uint32_t Length);

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartThread(void const * arg)
{
	uint8_t counter = 100;
	pRx = GetRxPacket();
	InitCommPackage(&TxPacket);
	uart2_init(kyLink_DecodeProcess);

	/* The Application layer has only to call USBD_Init to
	initialize the USB low level driver, the USB device library, the USB clock
	,pins and interrupt service routine (BSP) to start the Library*/
//	USBD_Init(&USB_Device_dev, &USR_desc, &USBD_CDC_cb, &USR_cb);

	while(counter --) {
		_delay_ms(10);
		if(GotNewData()) {//pRx->Packet.dev_id == HARD_DEV_ID && pRx->Packet.PacketData.FileInfo.FW_Type == FW_TYPE_IMU_APP &&
			if(pRx->Packet.msg_id == TYPE_UPGRADE_REQUEST && pRx->Packet.length == 16 &&
			   pRx->Packet.PacketData.FileInfo.Enc_Type == ENC_TYPE_PLAIN) {
				PackageNbr = pRx->Packet.PacketData.FileInfo.PacketNum;
				upgrade_flag = 1;
				break;
			}
		}
	}
	if(upgrade_flag) {
		TxPacket.Packet.msg_id = TYPE_UPGRADE_DEV_ACK;
		TxPacket.Packet.length = sizeof(DevResponseDef);
		TxPacket.Packet.PacketData.DevRespInfo.Dev_State = InErasing;

		/* Unlock the Flash to enable the flash control register access *************/
		FLASH_Unlock();
		/* Clear pending flags (if any) */
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
		/* Define the number of page to be erased */
//		NbrOfPage = (FLASH_USER_END_ADDR - FLASH_USER_START_ADDR) / FLASH_PAGE_SIZE;
		for(uint32_t EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter ++) {
			FLASH_ErasePage(APPLICATION_ADDRESS + (FLASH_PAGE_SIZE * EraseCounter));
//			_delay_ms(50);
			TxPacket.Packet.PacketData.DevRespInfo.reserve[0] = NbrOfPage;
			TxPacket.Packet.PacketData.DevRespInfo.reserve[1] = EraseCounter + 1;
			SendTxPacket(&TxPacket);
		}

		PacketSendStartTime = _Get_Millis();
		PacketSendTimeOut = 50;
		PackageRecNbr = 0;
		PacketDataInCacheIndex = 0;
		FlashProgramIndex = 0;

		for(;;) {
			if(GotNewData()) {
				if(pRx->Packet.msg_id == TYPE_UPGRADE_DATA && pRx->Packet.PacketData.PacketInfo.PacketID == PackageRecNbr) {
					PacketDataInCacheIndex = PackageRecNbr % PACKAGE_NUM_PER_CACHE;
					PackageRecNbr ++;
//					_delay_ms(10);
					memcpy((uint8_t *)&FlashProgramCache[PacketDataInCacheIndex * FILE_DATA_CACHE], pRx->Packet.PacketData.PacketInfo.PacketData, pRx->Packet.PacketData.PacketInfo.PacketLen);
					if(PacketDataInCacheIndex == (PACKAGE_NUM_PER_CACHE - 1) || PackageRecNbr == PackageNbr) {
						if(PackageRecNbr == PackageNbr) {
							FLASH_If_ProgramWords(APPLICATION_ADDRESS + FlashProgramIndex * USER_FLASH_PROGRAM_CACHE, FlashProgramCache,
									PacketDataInCacheIndex * FILE_DATA_CACHE + pRx->Packet.PacketData.PacketInfo.PacketLen);
							UpgradeComplete = 1;
						} else {
							FLASH_If_ProgramWords(APPLICATION_ADDRESS + FlashProgramIndex * USER_FLASH_PROGRAM_CACHE, FlashProgramCache,
									USER_FLASH_PROGRAM_CACHE);
							FlashProgramIndex ++;
						}
					}
					TxPacket.Packet.PacketData.DevRespInfo.Dev_State = Upgrading;
					TxPacket.Packet.PacketData.DevRespInfo.reserve[0] = PackageRecNbr;
					TxPacket.Packet.PacketData.DevRespInfo.reserve[1] = PackageRecNbr >> 8;
					TxPacket.Packet.PacketData.DevRespInfo.reserve[2] = PackageRecNbr >> 16;
					TxPacket.Packet.PacketData.DevRespInfo.reserve[3] = PackageRecNbr >> 24;
					SendTxPacket(&TxPacket);
					PacketSendStartTime = _Get_Millis();
					PacketSendTimeOut = 20;
				}
			}
			if(_Get_Millis() - PacketSendStartTime >= PacketSendTimeOut) {
				TxPacket.Packet.PacketData.DevRespInfo.Dev_State = Upgrading;
				TxPacket.Packet.PacketData.DevRespInfo.reserve[0] = PackageRecNbr;
				TxPacket.Packet.PacketData.DevRespInfo.reserve[1] = PackageRecNbr >> 8;
				TxPacket.Packet.PacketData.DevRespInfo.reserve[2] = PackageRecNbr >> 16;
				TxPacket.Packet.PacketData.DevRespInfo.reserve[3] = PackageRecNbr >> 24;
				SendTxPacket(&TxPacket);
				PacketSendStartTime = _Get_Millis();
				if(UpgradeComplete == 1) {
					break;
				}
			}
		}
		FLASH_Lock();
	}

	/* Check Vector Table: Test if user code is programmed starting from address
		"APPLICATION_ADDRESS" */
//	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000) {
//		/* Jump to user application */
//		uint32_t JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
//		Jump_To_Application = (pFunction) JumpAddress;
//		/* Initialize user application's Stack Pointer */
//		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
//		Jump_To_Application();
//	}
	for(;;) {

	}

//	sEE_Init();

//	for(;;) {
//		if(upgrade_flag) {
//			SendTxPacket(&TxPacket);
//			if(USBD_isEnabled()) {
//			uart2_TxBytesDMA((uint8_t *)"yyyyy\n", 6);
//			}
//		} else {
//			if(USBD_isEnabled()) {
//				USB_CDC_SendBufferFast((uint8_t *)"nnnnn\n", 6);
//			}
//			uart2_TxBytesDMA((uint8_t *)"nnnnn\n", 6);
//		}

//		_delay_ms(200);
}

FLASH_Status FLASH_If_ProgramWords(uint32_t Address, uint8_t *pData, uint32_t Length)
{
	FLASH_Status status = FLASH_COMPLETE;
	for(uint32_t programcounter = 0; programcounter < Length; programcounter += 4) {
//		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
		if((status = FLASH_ProgramWord(Address + programcounter, *(__IO uint32_t *)(pData + programcounter))) != FLASH_COMPLETE)
			break;
	}
	return status;
}

//uint32_t sEE_TIMEOUT_UserCallback(void)
//{
//	uart2_TxBytesDMA((uint8_t *)"IIC ERR.\n", 9);
//	return 0;
//}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
	  uart2_TxBytesDMA((uint8_t *)"FSH ERR.\n", 9);
	  _delay_ms(500);
  }
}
