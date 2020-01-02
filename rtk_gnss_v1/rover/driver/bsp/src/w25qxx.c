/*
 * w25qxx.c
 *
 *  Created on: Dec 25, 2019
 *      Author: kychu
 */

#include "w25qxx.h"

#define W25QXX_WCACHE_SIZE                       (8)
#define W25QXX_RCACHE_SIZE                       (4096)

#if FREERTOS_ENABLED
static uint8_t *w25qxx_wcache;
static uint8_t *w25qxx_rcache;
#else
static uint8_t w25qxx_wcache[W25QXX_WCACHE_SIZE];
static uint8_t w25qxx_rcache[W25QXX_RCACHE_SIZE];
#endif /* FREERTOS_ENABLED */

static uint8_t w25qxx_read_sr(void);
//static void w25qxx_write_sr(uint8_t sr);
static void w25qxx_wait_busy(void);
static status_t w25qxx_write_page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
static status_t w25qxx_write_no_check(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

status_t w25qxx_init(void)
{
  status_t ret;
  ret = flashif_init();
  if(ret != status_ok) return ret;
#if FREERTOS_ENABLED
  w25qxx_wcache = kmm_alloc(W25QXX_WCACHE_SIZE);
  if(w25qxx_wcache == NULL) return status_nomem;
  w25qxx_rcache = kmm_alloc(W25QXX_RCACHE_SIZE);
  if(w25qxx_rcache == NULL) return status_nomem;
#else
#endif /* FREERTOS_ENABLED */
  memset(w25qxx_wcache, 0, W25QXX_WCACHE_SIZE);
  memset(w25qxx_rcache, 0, W25QXX_RCACHE_SIZE);

  w25qxx_wcache[0] = 0x66;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);
  if(ret != status_ok) return ret;
  flash_delay(1);
  w25qxx_wcache[0] = 0x99;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);

  return ret;
}

/**
  * @brief  Read SPI_Flash Status Register
  * @param  None
  * @retval Register value.
  */
static uint8_t w25qxx_read_sr(void)
{
  w25qxx_wcache[0] = W25QXX_ReadStatusReg;
  w25qxx_wcache[1] = 0xFF;
  flashif_select(0);
  flashif_txrx_bytes(w25qxx_wcache, w25qxx_rcache, 2);
  flashif_deselect(0);
  return w25qxx_rcache[1];
}

/**
  * @brief  Write SPI_Flash Status Register.
  * @param  SR : Data to Write.
  * @retval None
  */
//static void w25qxx_write_sr(uint8_t sr)
//{
//  w25qxx_wcache[0] = W25QXX_WriteStatusReg;
//  w25qxx_wcache[1] = sr;
//  flashif_select(0);
//  flashif_tx_bytes(w25qxx_wcache, 2);
//  flashif_deselect(0);
//}

/**
  * @brief  SPI_Flash Write Enable.
  * @param  None
  * @retval None
  */
status_t w25qxx_write_enable(void)
{
  status_t ret;
  w25qxx_wcache[0] = W25QXX_WriteEnable;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
//  ret = flashif_txrx_bytes(w25qxx_wcache, w25qxx_rcache, 1);
  flashif_deselect(0);
  return ret;
}

//status_t w25qxx_write_protect(void)
//{
//  status_t ret;
//  w25qxx_wcache[0] = W25QXX_WriteStatusReg;
//  w25qxx_wcache[1] = 0x80; // SRP BIT
//  flashif_select(0);
//  ret = flashif_tx_bytes(w25qxx_wcache, 2);
//  flashif_deselect(0);
//  return ret;
//}

status_t w25qxx_write_unprotect(void)
{
  status_t ret;
  w25qxx_wait_busy();
  ret = w25qxx_write_enable();
  if(ret != status_ok) return ret;
  w25qxx_wcache[0] = W25QXX_WriteStatusReg;
  w25qxx_wcache[1] = 0x00; // SRP BIT
  w25qxx_wcache[2] = 0x00;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 3);
  flashif_deselect(0);
  w25qxx_wait_busy();
  return ret;
}

/**
  * @brief  SPI_Flash Write Disable.
  * @param  None
  * @retval None
  */
status_t w25qxx_write_disable(void)
{
  status_t ret;
  w25qxx_wcache[0] = W25QXX_WriteDisable;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);
  return ret;
}

/**
  * @brief  Read SPI_Flash's ID.
  * @param  None
  * @retval SPI_Flash's ID.
  */
status_t w25qxx_read_id(uint8_t *id)
{
  status_t ret;
  w25qxx_wcache[0] = 0x9F;
  w25qxx_wcache[1] = 0xFF;
  w25qxx_wcache[2] = 0xFF;
  w25qxx_wcache[3] = 0xFF;
  flashif_select(0);
  ret = flashif_txrx_bytes(w25qxx_wcache, w25qxx_rcache, 4);
  flashif_deselect(0);
  id[0] = w25qxx_rcache[1];
  id[1] = w25qxx_rcache[2];
  id[2] = w25qxx_rcache[3];
  return ret;
}

/**
  * @brief  Reads a block of data from the FLASH.
  * @param  pBuffer: pointer to the buffer that receives the data read from the FLASH.
  * @param  ReadAddr: SPI_FLASH's internal address to read from.
  * @param  NumByteToRead: number of bytes to read from the SPI FLASH.
  * @retval None
  */
status_t w25qxx_read_bytes(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  status_t ret = status_ok;
  w25qxx_wait_busy();

  w25qxx_wcache[0] = W25QXX_FastReadData;//W25QXX_ReadData;
  w25qxx_wcache[1] = (ReadAddr >> 16) & 0xFF;
  w25qxx_wcache[2] = (ReadAddr >> 8) & 0xFF;
  w25qxx_wcache[3] = ReadAddr & 0xFF;
  w25qxx_wcache[4] = 0xFF;

  flashif_select(0);
  if(w25qxx_wcache[0] == W25QXX_FastReadData)
    ret = flashif_tx_bytes(w25qxx_wcache, 5);
  else
    ret = flashif_tx_bytes(w25qxx_wcache, 4);
  if(ret == status_ok)
    ret = flashif_txrx_bytes(pBuffer, pBuffer, NumByteToRead);
  flashif_deselect(0);
  return ret;
}

/**
  * @brief  Writes a block of data to SPI Flash at specified page.
  * @param  pBuffer: pointer to the buffer that receives the data read from the SPI FLASH.
  * @param  WriteAddr: FLASH's internal address to read from.(<! less than 256 >)
  * @param  NumByteToRead: number of bytes to read from the SPI FLASH.
  * @retval None
  */
static status_t w25qxx_write_page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  status_t ret = status_ok;

  w25qxx_wait_busy();
  w25qxx_write_enable();

  w25qxx_wcache[0] = W25QXX_PageProgram;
  w25qxx_wcache[1] = (WriteAddr >> 16) & 0xFF;
  w25qxx_wcache[2] = (WriteAddr >> 8) & 0xFF;
  w25qxx_wcache[3] = WriteAddr & 0xFF;

  flashif_select(0);

  ret = flashif_tx_bytes(w25qxx_wcache, 4);
  if(ret == status_ok)
    ret = flashif_tx_bytes(pBuffer, NumByteToWrite);
  flashif_deselect(0);

  return ret;
}

/**
  * @brief  Writes a block of data to SPI Flash(no checksum).
  * @param  pBuffer: pointer to the buffer that receives the data read from the SPI FLASH.
  * @param  WriteAddr: FLASH's internal address to read from.(<! less than 65535 >)
  * @param  NumByteToRead: number of bytes to read from the SPI FLASH.
  * @retval None
  */
static status_t w25qxx_write_no_check(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  status_t ret = status_ok;
  uint16_t Page_Remain;
  Page_Remain = 0x100 - (WriteAddr & 0xFF);
  if(NumByteToWrite <= Page_Remain)
    Page_Remain = NumByteToWrite;

  do {
//    ky_info("write %d bytes to 0x%x\n", Page_Remain, WriteAddr);
//    ky_info("buffer: %d %d %d %d %d\n", pBuffer[0], pBuffer[1], pBuffer[2], pBuffer[3], pBuffer[4]);
    ret = w25qxx_write_page(pBuffer, WriteAddr, Page_Remain);
    if(ret != status_ok) return ret;
    if(NumByteToWrite == Page_Remain) {/* Write OK. */
      break;
    } else {
      pBuffer += Page_Remain;
      WriteAddr += Page_Remain;
      NumByteToWrite -= Page_Remain;
      if(NumByteToWrite > 0x100)
        Page_Remain = 0x100;
      else
        Page_Remain = NumByteToWrite;
    }
  } while(1);
  return ret;
}

/**
  * @brief  Writes a block of data to SPI Flash(will erase sector).
  * @param  pBuffer: pointer to the buffer that receives the data read from the SPI FLASH.
  * @param  WriteAddr: FLASH's internal address to read from.(<! less than 65535 >)
  * @param  NumByteToRead: number of bytes to read from the SPI FLASH.
  * @retval None
  */
status_t w25qxx_write_bytes(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  status_t ret = status_ok;
  int sec_idx, sec_off, sec_remain;
  uint8_t *p;

  sec_idx = WriteAddr >> 12;
  sec_off = WriteAddr & 0xFFF;
  sec_remain = 0x1000 - sec_off;

  if(NumByteToWrite <= sec_remain)
    sec_remain = NumByteToWrite;

//  ky_info("sector index  %d\n", sec_idx);
//  ky_info("sector offset %d\n", sec_off);
//  ky_info("sector remain %d\n", sec_remain);

  do {
	if(sec_off != 0) {
      ret = w25qxx_read_bytes(w25qxx_rcache, sec_idx << 12, sec_off);
      if(ret != status_ok) return ret;
      memcpy(w25qxx_rcache + sec_off, pBuffer, sec_remain);
      p = w25qxx_rcache;
	} else {
      p = pBuffer;
	}
//    ky_info("erase sector %d\n", sec_idx);
    ret = w25qxx_erase_sector(sec_idx);
    if(ret != status_ok) return ret;
//    ky_info("write size %d\n", sec_off + sec_remain);
    ret = w25qxx_write_no_check(p, sec_idx << 12, sec_off + sec_remain);
    if(ret != status_ok) return ret;

    if(NumByteToWrite == sec_remain)
      break;
    else {
      sec_idx ++;
      sec_off = 0;

      pBuffer += sec_remain;
      WriteAddr += sec_remain;
      NumByteToWrite -= sec_remain;
      if(NumByteToWrite > 4096)
        sec_remain = 4096;
      else
        sec_remain = NumByteToWrite;
    }
  } while(1);
  return ret;
}

/**
  * @brief  Erases the entire FLASH.(approximately 25s ?)
  * @param  None.
  * @retval None.
  */
status_t w25qxx_erase_chip(void)
{
  status_t ret;
  w25qxx_wait_busy();
  ret = w25qxx_write_enable();
  if(ret != status_ok) return ret;

  w25qxx_wcache[0] = W25QXX_ChipErase;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);

//  w25qxx_wait_busy();
  do {
    flash_delay(100); // check busy every 100ms;
  } while((w25qxx_read_sr() & 0x01) == 0x01);
  return ret;
}

/**
  * @brief  Erases the specified FLASH sector(4KB).
  * @param  SectorAddr: address of the sector to erase.(0 - 512)
  * @retval None
  */
status_t w25qxx_erase_sector(uint32_t sector_idx)
{
  status_t ret = status_ok;
  sector_idx <<= 12;

  w25qxx_wait_busy();
  ret = w25qxx_write_enable();
  if(ret != status_ok) return ret;

  w25qxx_wcache[0] = W25QXX_SectorErase;
  w25qxx_wcache[1] = (sector_idx >> 16) & 0xFF;
  w25qxx_wcache[2] = (sector_idx >> 8) & 0xFF;
  w25qxx_wcache[3] = sector_idx & 0xFF;

  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 4);
  flashif_deselect(0);

  w25qxx_wait_busy();

  return ret;
}

/**
  * @brief  Check SPI Flash Busy.
  * @param  None
  * @retval None
  */
static void w25qxx_wait_busy(void)
{
  uint8_t status;
  do {
    status = w25qxx_read_sr();
  } while((status & 0x01) == 0x01);
}

/**
  * @brief  Make SPI Flash Power Down.
  * @param  None
  * @retval None
  */
status_t w25qxx_power_down(void)
{
  status_t ret = status_ok;
  w25qxx_wcache[0] = W25QXX_PowerDown;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);
  /* wait 1 ms */
  flash_delay(1);
  return ret;
}

/**
  * @brief  Make SPI Flash Wake Up.
  * @param  None
  * @retval None
  */
status_t w25qxx_wakeup(void)
{
  status_t ret = status_ok;
  w25qxx_wcache[0] = W25QXX_ReleasePowerDown;
  flashif_select(0);
  ret = flashif_tx_bytes(w25qxx_wcache, 1);
  flashif_deselect(0);
  /* wait 1 ms */
  flash_delay(1);
  return ret;
}
