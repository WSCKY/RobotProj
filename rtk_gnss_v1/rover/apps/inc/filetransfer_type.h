/*
 * filetransfer_type.h
 *
 *  Created on: Jan 3, 2020
 *      Author: kychu
 */

#ifndef INC_FILETRANSFER_TYPE_H_
#define INC_FILETRANSFER_TYPE_H_

#include "ff.h"
#include "kyLinkMacros.h"

#define FILETRANSFER_CACHE_SIZE                  (100)

#define FILETRANSFER_FILENAME_LEN                (64)

typedef enum {
  ENC_PLAIN   = 0,
  ENC_XOR     = 1,
  ENC_AES_ECB = 2,
} FileEncryptType;

typedef enum {
  F_OPT_NULL   = 0, // null operation.
  F_OPT_SEND   = 1, // send file to lower.
  F_OPT_RECV   = 2, // send file to upper.
  F_OPT_LIST   = 3, // get directory content.
  F_OPT_CREATE = 4, // create file/directory.
  F_OPT_DELETE = 5, // delete file/directory.
} FileOperateType;

typedef uint8_t FileOperateState;

__PACK_BEGIN typedef struct {
  FileOperateType OptCmd;
  union {
    struct {
      FileEncryptType EncType;
      uint8_t FileAttr;
    } FileInfo;
    struct {

    } ListFile;
    struct {
      uint8_t FileAttr;
    } CreateObj;
    uint8_t reserve[4];
  };
  char Filename[FILETRANSFER_FILENAME_LEN];
} __PACK_END FileTransReq_T;

__PACK_BEGIN typedef struct {
  union {
    struct {
      uint32_t PackID;
      uint32_t DataLen;
      uint8_t fData[FILETRANSFER_CACHE_SIZE];
    } FileData;
    struct {
      uint8_t FileId;
      uint8_t FileAttr;
      uint8_t FilePath[FILETRANSFER_FILENAME_LEN / 2];
      uint8_t FileName[FILETRANSFER_FILENAME_LEN / 2];
    } FileInfo;
  };
} __PACK_END FileTransData_T;

__PACK_BEGIN typedef struct {
  FileOperateType OptCmd;
  FileOperateState OptSta;
  union {
    struct {
      uint32_t PackID;
    } PackInfo;
    struct {
      uint32_t FileId;
    } FileInfo;
  };
} __PACK_END FileTransAck_T;

#endif /* INC_FILETRANSFER_TYPE_H_ */
