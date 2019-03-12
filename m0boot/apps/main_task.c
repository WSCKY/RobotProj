/*
 * main_task.c
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#include "main_task.h"
#include "string.h"

CommPackageDef *pRx;
CommPackageDef TxPacket;
uint8_t upgrade_flag = 0;
uint32_t NbrOfPage = (FLASH_END_ADDRESS - APPLICATION_ADDRESS) / FLASH_PAGE_SIZE;

uint32_t PackageNbr = 0, PackageRecNbr = 0;

pFunction Jump_To_Application;

uint8_t PacketDataInCacheIndex = 0;
uint8_t FlashProgramIndex = 0;
extern uint8_t _boot_cache[BOOT_CACHE_SIZE];

uint32_t PacketSendStartTime = 0;
uint32_t PacketSendTimeOut = 0;

uint8_t UpgradeComplete = 0;

//static const uint8_t jmp_err[7] = "FAILED\n";

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
		/* erase the pages */
		for(uint32_t EraseCounter = 0; EraseCounter < NbrOfPage; EraseCounter ++) {
			FLASH_ErasePage(APPLICATION_ADDRESS + (FLASH_PAGE_SIZE * EraseCounter));
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
					memcpy((uint8_t *)&_boot_cache[PacketDataInCacheIndex * FILE_DATA_CACHE], pRx->Packet.PacketData.PacketInfo.PacketData, pRx->Packet.PacketData.PacketInfo.PacketLen);
					if(PacketDataInCacheIndex == (PACKAGE_NUM_PER_CACHE - 1) || PackageRecNbr == PackageNbr) {
						if(PackageRecNbr == PackageNbr) {
							FLASH_If_ProgramWords(APPLICATION_ADDRESS + FlashProgramIndex * BOOT_CACHE_SIZE, _boot_cache,
									PacketDataInCacheIndex * FILE_DATA_CACHE + pRx->Packet.PacketData.PacketInfo.PacketLen);
							UpgradeComplete = 1;
						} else {
							FLASH_If_ProgramWords(APPLICATION_ADDRESS + FlashProgramIndex * BOOT_CACHE_SIZE, _boot_cache,
									BOOT_CACHE_SIZE);
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
					_delay_ms(20);
					break;
				}
			}
		}
		FLASH_Lock();
	}

	uart2_deinit();

	/* Check Vector Table: Test if user code is programmed starting from address
		"APPLICATION_ADDRESS" */
	if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000) {
		/* Jump to user application */
		uint32_t JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}

	for(;;) {
//		_delay_ms(500);
//		uart2_TxBytes((uint8_t *)jmp_err, 7);
	}
}

FLASH_Status FLASH_If_ProgramWords(uint32_t Address, uint8_t *pData, uint32_t Length)
{
	FLASH_Status status = FLASH_COMPLETE;
	for(uint32_t programcounter = 0; programcounter < Length; programcounter += 4) {
		if((status = FLASH_ProgramWord(Address + programcounter, *(__IO uint32_t *)(pData + programcounter))) != FLASH_COMPLETE)
			break;
	}
	return status;
}
