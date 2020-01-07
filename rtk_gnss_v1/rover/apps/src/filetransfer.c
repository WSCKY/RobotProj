/*
 * filetransfer.c
 *
 *  Created on: Jan 3, 2020
 *      Author: kychu
 */

#include "filetransfer.h"

static FileTransReq_T currentReq;
static FileTransData_T currentData;
static FileTransAck_T currentAck;

static osSemaphoreId f_req_sem;
static osSemaphoreId f_dat_sem;
static osSemaphoreId f_ack_sem;

static uint32_t _task_running = 0;

static void file_list_content(void);
static void file_upload(void);
static void file_download(void);
static void file_create(void);
static void file_delete(void);

void transfile_task(void const *argument)
{
  currentReq.OptCmd = F_OPT_NULL;
  osSemaphoreDef(F_REQ_SEM);
  osSemaphoreDef(F_DAT_SEM);
  osSemaphoreDef(F_ACK_SEM);
  f_req_sem = osSemaphoreCreate(osSemaphore(F_REQ_SEM), 1);
  f_dat_sem = osSemaphoreCreate(osSemaphore(F_DAT_SEM), 1);
  f_ack_sem = osSemaphoreCreate(osSemaphore(F_ACK_SEM), 1);
  if(f_req_sem == NULL || f_dat_sem == NULL || f_ack_sem == NULL) {
    ky_err("file transfer task exit!.\n");
    osSemaphoreDelete(f_req_sem);
    osSemaphoreDelete(f_dat_sem);
    osSemaphoreDelete(f_ack_sem);
    vTaskDelete(NULL);
  }
  _task_running = 1;
  for(;;) {
    if(osSemaphoreWait(f_req_sem, osWaitForever) == osOK) {
      switch(currentReq.OptCmd) {
      case F_OPT_NULL: break;
      case F_OPT_SEND: file_download(); break;
      case F_OPT_RECV: file_upload(); break;
      case F_OPT_LIST: file_list_content(); break;
      case F_OPT_CREATE: file_create(); break;
      case F_OPT_DELETE: file_delete(); break;
      }
    }
  }
}

static void file_list_content(void)
{
  FRESULT ret;
  DIR *dir;
  FILINFO *fno;
  ky_info("list dir %s.\n", currentReq.Filename);
  dir = kmm_alloc(sizeof(DIR));
  fno = kmm_alloc(sizeof(FILINFO));
  if(dir == NULL || fno == NULL) {
    ky_err("no memory.\n");
    return;
  } else {
    ret = f_opendir(dir, (const TCHAR *)currentReq.Filename);
    if(ret != FR_OK) {
      ky_err("open %s failed.\n", currentReq.Filename);
    } else {
      do {
        ret = f_readdir(dir, fno);
        if(ret != FR_OK) {
          ky_err("read %s failed.\n", currentReq.Filename);
          break;
        } else if(fno->fname[0] == 0) {
          ky_err("read %s done.\n", currentReq.Filename);
          break;
        } else {
          ky_info("%s: name: %s, size: %d, altname: %s.\n", currentReq.Filename, fno->fname, fno->fsize, fno->altname);
        }
      } while(1);
      ret = f_closedir(dir);
      if(ret != FR_OK) {
        ky_err("close directory failed.\n");
      }
    }
  }

  kmm_free(dir);
  kmm_free(fno);

  currentReq.OptCmd = F_OPT_NULL;
}

static void file_upload(void)
{
  ky_info("send file %s.\n", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
}

static void file_download(void)
{
  ky_info("load file %s.\n", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
}

static void file_create(void)
{
  ky_info("create %s.\n", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  if((currentReq.CreateObj.FileAttr & AM_DIR) == 0) {
    ky_info("create FILE.\n");
    FIL *file = kmm_alloc(sizeof(FIL));
    if(file == NULL) {
      ky_err("no memory.\n");
      return;
    } else {
      if(f_open(file, (const TCHAR *)currentReq.Filename, FA_CREATE_ALWAYS) != FR_OK) {
        ky_err("failed to create file.\n");
      } else {
        if(f_close(file) != FR_OK) {
          ky_err("close failed.\n");
        }
      }
    }
    kmm_free(file);
  } else {
    ky_info("create DIR.\n");
    if(f_mkdir((const TCHAR *)currentReq.Filename) != FR_OK) {
      ky_err("create DIR failed.\n");
    }
  }
}

static void file_delete(void)
{
  ky_info("delete %s.\n", currentReq.Filename);
  currentAck.OptCmd = F_OPT_DELETE;
  currentAck.OptSta = f_unlink((const TCHAR *)currentReq.Filename);
  mesg_send_mesg(currentAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T));
  currentReq.OptCmd = F_OPT_NULL;
}

void filetransfer_cmd_process(kyLinkBlockDef *pRx)
{
  if(_task_running == 0) return;
  if(pRx->msg_id == FILE_TRANS_REQ_MSG) {
    currentReq = *((FileTransReq_T *)pRx->buffer);
    osSemaphoreRelease(f_req_sem);
  } else if(pRx->msg_id == FILE_TRANS_DATA_MSG) {
    currentData = *((FileTransData_T *)pRx->buffer);
    osSemaphoreRelease(f_dat_sem);
  } else if(pRx->msg_id == FILE_TRANS_ACK_MSG) {
    currentAck = *((FileTransAck_T *)pRx->buffer);
    osSemaphoreRelease(f_ack_sem);
  }
}
