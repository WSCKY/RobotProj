/*
 * filetransfer.c
 *
 *  Created on: Jan 3, 2020
 *      Author: kychu
 */

#include "filetransfer.h"

static const char *TAG = "FT";

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

static int wait_for_data(uint32_t timeout);
static int wait_for_ack(FileOperateType opt, uint32_t timeout);

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
    ky_err(TAG, "file transfer task exit!.");
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
  int n_len, f_idx;
  DIR *dir;
  FILINFO *fno;
  FileTransAck_T OperateAck;
  ky_info(TAG, "list dir %s.", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  OperateAck.OptCmd = F_OPT_LIST;
  dir = kmm_alloc(sizeof(DIR));
  fno = kmm_alloc(sizeof(FILINFO));
  if(dir == NULL || fno == NULL) {
    ky_err(TAG, "no memory.");
    OperateAck.OptSta = 20; // memory alloc failed.
  } else {
    ret = f_opendir(dir, (const TCHAR *)currentReq.Filename);
    if(ret != FR_OK) {
      ky_err(TAG, "open %s failed.", currentReq.Filename);
    } else {
      n_len = 0;
      f_idx = 0;
      do {
        ret = f_readdir(dir, fno);
        if(ret != FR_OK) {
          ky_err(TAG, "read %s failed.", currentReq.Filename);
          break;
        } else if(fno->fname[0] == 0) {
          ky_err(TAG, "read %s done.", currentReq.Filename);
          break;
        } else {
          ky_info(TAG, "%s: name: %s, size: %ld, altname: %s.", currentReq.Filename, fno->fname, fno->fsize, fno->altname);
          currentData.FileInfo.FileId = f_idx ++;
          currentData.FileInfo.FileAttr = fno->fattrib;
          n_len = MIN(FILETRANSFER_FILENAME_LEN / 2, strlen(fno->fname));
          memcpy(currentData.FileInfo.FileName, fno->fname, n_len);
          if(n_len < FILETRANSFER_FILENAME_LEN / 2) currentData.FileInfo.FileName[n_len] = 0;
          n_len = MIN(FILETRANSFER_FILENAME_LEN / 2, strlen(currentReq.Filename));
          memcpy(currentData.FileInfo.FilePath, currentReq.Filename, n_len);
          if(n_len < FILETRANSFER_FILENAME_LEN / 2) currentData.FileInfo.FilePath[n_len] = 0;
          n_len = 3; // try 3 times.
          do {
            mesg_send_mesg(&currentData, FILE_TRANS_DATA_MSG, sizeof(FileTransData_T));
          } while((wait_for_ack(F_OPT_LIST, 1000) != 0) && ((-- n_len) > 0)); // wait 1s for ack.
        }
      } while(1);
      ret = f_closedir(dir);
      if(ret != FR_OK) {
        ky_err(TAG, "close directory failed.");
      }
    }
    OperateAck.OptSta = ret;
  }

  kmm_free(dir);
  kmm_free(fno);

  mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // notify upper
}

static void file_upload(void)
{
  FRESULT ret;
  int d_idx, times, ack_ret;
  FIL *file;
  UINT bytes_rd;
  FileTransAck_T OperateAck;
  ky_info(TAG, "send file %s.", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  OperateAck.OptCmd = F_OPT_RECV;
  file = kmm_alloc(sizeof(FIL));
  if(file == NULL) {
    ky_err(TAG, "no memory.");
    OperateAck.OptSta = 20; // memory alloc failed.
  } else {
    ret = f_open(file, (const TCHAR *)currentReq.Filename, FA_READ);
    if(ret != FR_OK) {
      ky_err(TAG, "failed to read file.");
    } else {
      d_idx = 0;
      do {
        ret = f_read(file, currentData.FileData.fData, FILETRANSFER_CACHE_SIZE, (UINT *)&bytes_rd);
        if(bytes_rd == 0 || ret != FR_OK) {
          break;
        } else {
          currentData.FileData.PackID = d_idx ++;
          currentData.FileData.DataLen = bytes_rd;
          times = 3;
          do {
            mesg_send_mesg(&currentData, FILE_TRANS_DATA_MSG, sizeof(FileTransData_T));
            ack_ret = wait_for_ack(F_OPT_RECV, 1000); // wait for ack.
          } while((ack_ret != 0) && ((-- times) > 0));
          if(times == 0 && ack_ret != 0) {
            ret = FR_TIMEOUT;
            break; // timeout, exit.
          }
        }
      } while(1);
      ret = f_close(file);
      if(ret != FR_OK) {
        ky_err(TAG, "failed to close file.");
      }
    }
    OperateAck.OptSta = ret;
    kmm_free(file);
  }
  mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // notify upper
}

static void file_download(void)
{
  FRESULT ret;
  int p_idx, times, ack_ret;
  FIL *file;
  UINT bytes_wr;
  FileTransAck_T OperateAck;
  ky_info(TAG, "load file %s.", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  OperateAck.OptCmd = F_OPT_SEND;
  file = kmm_alloc(sizeof(FIL));
  if(file == NULL) {
    ky_err(TAG, "no memory.");
    OperateAck.OptSta = 20; // memory alloc failed.
  } else {
    ret = f_open(file, (const TCHAR *)currentReq.Filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(ret != FR_OK) {
      ky_err(TAG, "failed to open file.");
    } else {
      p_idx = 0;
      do {
        OperateAck.PackInfo.PackID = p_idx ++;
        times = 3; // try 3 times.
        do {
          mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // s1: request data by lower first.
          ack_ret = wait_for_data(1000);
        } while((ack_ret != 0) && ((-- times) > 0)); // s2: wait for data received.
        if(times == 0 && ack_ret != 0) {
          ret = FR_TIMEOUT; // upper will check result of operation.
          break; // timeout, exit.
        } else {
          if(currentData.FileData.DataLen != 0) { // check if last package.
            ret = f_write(file, currentData.FileData.fData, currentData.FileData.DataLen, (void *)&bytes_wr);
            if(bytes_wr != currentData.FileData.DataLen || ret != FR_OK) {
              ky_err(TAG, "file write error. %d", ret);
              break;
            } // then request next package if all is successful.
          } else {
            break; // if we got a null package, exit.
          }
        }
      } while(1);
      ky_info(TAG, "wrote %d packages.", p_idx);
      ret = f_close(file);
      if(ret != FR_OK) {
        ky_err(TAG, "failed to close file.");
      }
    }
    OperateAck.OptSta = ret;
    kmm_free(file);
  }
  OperateAck.PackInfo.PackID = 0xFFFFFFFF;
  mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // notify upper
}

static void file_create(void)
{
  FRESULT ret;
  FileTransAck_T OperateAck;
  ky_info(TAG, "create %s.", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  OperateAck.OptCmd = F_OPT_CREATE;
  if((currentReq.CreateObj.FileAttr & AM_DIR) == 0) {
    ky_info(TAG, "create FILE.");
    FIL *file = kmm_alloc(sizeof(FIL));
    if(file == NULL) {
      ky_err(TAG, "no memory.");
      OperateAck.OptSta = 20; // memory alloc failed.
    } else {
      ret = f_open(file, (const TCHAR *)currentReq.Filename, FA_CREATE_ALWAYS);
      if(ret != FR_OK) {
        ky_err(TAG, "failed to create file.");
      } else {
        ret = f_close(file);
        if(ret != FR_OK) {
          ky_err(TAG, "close failed.");
        }
      }
      OperateAck.OptSta = ret;
    }
    kmm_free(file);
  } else {
    ky_info(TAG, "create DIR.");
    ret = f_mkdir((const TCHAR *)currentReq.Filename);
    if(ret != FR_OK) {
      ky_err(TAG, "create DIR failed.");
    }
    OperateAck.OptSta = ret;
  }
  mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // notify upper
}

static void file_delete(void)
{
  FileTransAck_T OperateAck;
  ky_info(TAG, "delete %s.", currentReq.Filename);
  currentReq.OptCmd = F_OPT_NULL;
  OperateAck.OptCmd = F_OPT_DELETE;
  OperateAck.OptSta = f_unlink((const TCHAR *)currentReq.Filename);
  mesg_send_mesg(&OperateAck, FILE_TRANS_ACK_MSG, sizeof(FileTransAck_T)); // notify upper
}

static int wait_for_data(uint32_t timeout)
{
  uint32_t time_start;
  time_start = xTaskGetTickCountFromISR();
  currentData.FileData.PackID = 0xFFFFFFFF;
  do {
    osSemaphoreWait(f_dat_sem, 5);
  } while((currentData.FileData.PackID == 0xFFFFFFFF) && (xTaskGetTickCountFromISR() - time_start < timeout));
  if(currentData.FileData.PackID == 0xFFFFFFFF) return -1;
  return 0;
}

static int wait_for_ack(FileOperateType opt, uint32_t timeout)
{
  uint32_t time_start;
  time_start = xTaskGetTickCountFromISR();
  currentAck.OptCmd = F_OPT_NULL;
  do {
    osSemaphoreWait(f_ack_sem, 5);
  } while((currentAck.OptCmd != opt) && (xTaskGetTickCountFromISR() - time_start < timeout));
  if(currentAck.OptCmd != opt) return -1;
  return 0;
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
