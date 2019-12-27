/*
 * w25qxx.h
 *
 *  Created on: Dec 25, 2019
 *      Author: kychu
 */

#ifndef BSP_INC_W25QXX_H_
#define BSP_INC_W25QXX_H_

#include "drivers.h"

#if FREERTOS_ENABLED
#define flash_delay                              osDelay
#else
#define flash_delay                              HAL_Delay
#endif /* FREERTOS_ENABLED */

/**
  * @brief  W25QXX JDEC_ID Type Definition
  */
#define W25_JEDEC_MANUFACTURER     0xef  /* Winbond manufacturer ID */
#define W25X_JEDEC_MEMORY_TYPE     0x30  /* W25X memory type */
#define W25Q_JEDEC_MEMORY_TYPE_A   0x40  /* W25Q memory type */
#define W25Q_JEDEC_MEMORY_TYPE_B   0x60  /* W25Q memory type */
#define W25Q_JEDEC_MEMORY_TYPE_C   0x50  /* W25Q memory type */

#define W25_JEDEC_CAPACITY_8MBIT   0x14  /* 256x4096  = 8Mbit memory capacity */
#define W25_JEDEC_CAPACITY_16MBIT  0x15  /* 512x4096  = 16Mbit memory capacity */
#define W25_JEDEC_CAPACITY_32MBIT  0x16  /* 1024x4096 = 32Mbit memory capacity */
#define W25_JEDEC_CAPACITY_64MBIT  0x17  /* 2048x4096 = 64Mbit memory capacity */
#define W25_JEDEC_CAPACITY_128MBIT 0x18  /* 4096x4096 = 128Mbit memory capacity */

/**
  * @brief  W25QXX Register Definition
  */
#define W25QXX_WriteEnable        0x06
#define W25QXX_WriteDisable       0x04
#define W25QXX_ReadStatusReg      0x05
#define W25QXX_WriteStatusReg     0x01
#define W25QXX_ReadData           0x03
#define W25QXX_FastReadData       0x0B
#define W25QXX_FastReadDual       0x3B
#define W25QXX_PageProgram        0x02
#define W25QXX_BlockErase         0xD8
#define W25QXX_SectorErase        0x20
#define W25QXX_ChipErase          0xC7
#define W25QXX_PowerDown          0xB9
#define W25QXX_ReleasePowerDown   0xAB
#define W25QXX_DeviceID           0xAB
#define W25QXX_ManufactDeviceID   0x90
#define W25QXX_JedecDeviceID      0x9F

status_t w25qxx_init(void);
status_t w25qxx_write_enable(void);
status_t w25qxx_write_disable(void);
status_t w25qxx_read_id(uint8_t *id);
status_t w25qxx_power_down(void);
status_t w25qxx_wakeup(void);
//status_t w25qxx_write_protect(void);
status_t w25qxx_write_unprotect(void);

status_t w25qxx_erase_chip(void);
status_t w25qxx_erase_sector(uint32_t sector_idx);
status_t w25qxx_read_bytes(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
status_t w25qxx_write_bytes(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

#endif /* BSP_INC_W25QXX_H_ */
