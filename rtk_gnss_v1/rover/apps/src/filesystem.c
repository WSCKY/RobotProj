/*
 * filesystem.c
 *
 *  Created on: Jan 2, 2020
 *      Author: kychu
 */

#include "filesystem.h"

static const char *TAG = "FS";

extern const Diskio_drvTypeDef mtd_driver;

static DWORD volume_total_size = 0;
static FATFS *fatfs;

status_t fatfs_mount(void)
{
  FRESULT ret;

  char *path = NULL;
  int mkfs_flag = 0;
  uint32_t free_clust = 0;

  uint8_t *workBuffer = NULL;

  path = kmm_alloc(4);
  fatfs = kmm_alloc(sizeof(FATFS));
  if(path == NULL || fatfs == NULL) {
    ky_err(TAG, "no enough memory.");
    return status_nomem;
  }

  ky_info(TAG, "link disk I/O.");
  ret = FATFS_LinkDriver(&mtd_driver, path);
  if(ret != FR_OK) {
    ky_err(TAG, "Link the disk I/O driver failed.");
    goto exit;
  }

mount:
  ky_info(TAG, "mount fatfs to %s", path);
  ret = f_mount(fatfs, (TCHAR const *)path, 1); // force mount immediately to get capacity of the disk.
  if(ret != FR_OK) {
    if(ret == FR_NO_FILESYSTEM && mkfs_flag == 0) {
      ky_warn(TAG, "warning: NO FS FOND! mkfatfs now ...");
      workBuffer = kmm_alloc(_MAX_SS);
      if(workBuffer == NULL) {
        ky_err(TAG, "no enough memory to mkfatfs, failed!");
        goto exit;
      }
      mkfs_flag ++; // only format once.
      ret = f_mkfs((TCHAR const *)path, FM_ANY, 0, workBuffer, _MAX_SS);
      if(ret != FR_OK) {
        ky_err(TAG, "make fatfs failed.");
        goto exit;
      }
      goto mount;
    }
    ky_err(TAG, "mount fatfs failed.");
    goto exit;
  }

  volume_total_size = (fatfs->n_fatent - 2) * fatfs->csize;
/* DEBUG INFORMATION */
  ky_info(TAG, "disk capacity: %ld.", volume_total_size);
  ret = f_getfree(path, (DWORD *)&free_clust, &fatfs);
  if(ret == FR_OK)
    ky_info(TAG, "disk free: %ld.", free_clust * fatfs->csize);
  else
    ky_info(TAG, "failed to get disk free info.");

  exit:
    kmm_free(path);
    if(ret != FR_OK) {
      kmm_free(fatfs);
      ky_info(TAG, "free FS object.");
    }
    kmm_free(workBuffer);

  if(ret != FR_OK) {
    return status_error;
  } else {
    return status_ok;
  }
}

status_t fatfs_usage(uint16_t *usage)
{
  FRESULT ret;
  uint32_t free_clust = 0;
  ret = f_getfree("0:/", (DWORD *)&free_clust, &fatfs);
  if(ret == FR_OK) {
    *usage = 100 - (free_clust * 100) / (fatfs->n_fatent - 2);
    return status_ok;
  }
  return status_error;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
