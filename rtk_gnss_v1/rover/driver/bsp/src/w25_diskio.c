/*
 * w25_diskio.c
 *
 *  Created on: Dec 27, 2019
 *      Author: kychu
 */

/* Includes ------------------------------------------------------------------*/
#include "drivers.h"
#include "ff_gen_drv.h"

static const char *TAG = "DISK";

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CONFIG_MTD_SECTOR_SIZE                   (4096)

#define CONFIG_SECTOR_SIZE_LSH                   (9)
#define CONFIG_SECTOR_SIZE                       (1 << CONFIG_SECTOR_SIZE_LSH) /* equals to _MAX_SS */
#define CONFIG_SECTOR_NUMBER                     (0x4000)
#define CONFIG_ERASE_BLOCK_NUM_SECTOR            (8) /* (CONFIG_MTD_SECTOR_SIZE / CONFIG_SECTOR_SIZE) */
/* Private variables ---------------------------------------------------------*/
/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS mtd_initialize(BYTE);
static DSTATUS mtd_status(BYTE);
static DRESULT mtd_read(BYTE, BYTE*, DWORD, UINT);
#if _USE_WRITE == 1
static DRESULT mtd_write (BYTE, const BYTE*, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
static DRESULT mtd_ioctl (BYTE, BYTE, void*);
#endif  /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef mtd_driver = {
  mtd_initialize,
  mtd_status,
  mtd_read,
#if  _USE_WRITE == 1
  mtd_write,
#endif /* _USE_WRITE == 1 */

#if  _USE_IOCTL == 1
  mtd_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes a Drive
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
static DSTATUS mtd_initialize(BYTE lun)
{
  if(w25qxx_init() != status_ok) {
    ky_err(TAG, "failed to initialize w25qxx.");
    return Stat;
  }

  Stat &= ~STA_NOINIT;
  return Stat;
}

/**
  * @brief  Gets Disk Status
  * @param  lun : not used
  * @retval DSTATUS: Operation status
  */
static DSTATUS mtd_status(BYTE lun)
{
  return Stat;
}

/**
  * @brief  Reads Sector(s)
  * @param  lun : not used
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
static DRESULT mtd_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  if(!count) return res;
  if(w25qxx_read_bytes((uint8_t *)buff, sector << CONFIG_SECTOR_SIZE_LSH, count << CONFIG_SECTOR_SIZE_LSH) == status_ok) {
    res = RES_OK;
  }
//  ky_info(TAG, "read 0x%x, 0x%x", sector << CONFIG_SECTOR_SIZE_LSH, count << CONFIG_SECTOR_SIZE_LSH);
  return res;
}

#if _USE_WRITE == 1
static DRESULT mtd_write (BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
  DRESULT res = RES_ERROR;
  if(w25qxx_write_bytes((uint8_t *)buff, sector << CONFIG_SECTOR_SIZE_LSH, count << CONFIG_SECTOR_SIZE_LSH) == status_ok) {
    res = RES_OK;
  }
//  ky_info(TAG, "write 0x%x, 0x%x", sector << CONFIG_SECTOR_SIZE_LSH, count << CONFIG_SECTOR_SIZE_LSH);
  return res;
}
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
static DRESULT mtd_ioctl (BYTE lun, BYTE cmd, void *buff)
{
  DRESULT res = RES_ERROR;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd) {
    /* Make sure that no pending write process */
    case CTRL_SYNC :
      res = RES_OK;
    break;

    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT :
      *(DWORD*)buff = CONFIG_SECTOR_NUMBER;
      res = RES_OK;
    break;

    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE :
      *(WORD*)buff = CONFIG_SECTOR_SIZE;
      res = RES_OK;
    break;

    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE :
      *(DWORD*)buff = CONFIG_ERASE_BLOCK_NUM_SECTOR;
      res = RES_OK;
    break;

    default:
      res = RES_PARERR;
    }

    return res;
}
#endif  /* _USE_IOCTL == 1 */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
