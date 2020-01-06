/*
 * filetransfer_type.h
 *
 *  Created on: Jan 3, 2020
 *      Author: kychu
 */

#ifndef INC_FILETRANSFER_TYPE_H_
#define INC_FILETRANSFER_TYPE_H_

#include "kyLinkMacros.h"

#define FILETRANSFER_CACHE_SIZE                  (100)

#define FILETRANSFER_FILENAME_LEN                (64)

typedef enum {
  ENC_PLAIN   = 0,
  ENC_XOR     = 1,
  ENC_AES_ECB = 2,
} FileEncryptType;

typedef enum {
  F_OPT_SEND   = 0, // send file to lower.
  F_OPT_RECV   = 1, // send file to upper.
  F_OPT_LIST   = 2, // get directory content.
  F_OPT_CREATE = 3, // create file/directory.
  F_OPT_DELETE = 4, // delete file/directory.
} FileOperateType;

typedef enum {
  F_OK = 0,    // OK
  F_ERROR = 1, // ERROR
} FileOperateState;

__PACK_BEGIN typedef struct {
  FileOperateType OptCmd;
  union {
    __PACK_BEGIN struct {
      FileEncryptType EncType;

    } FileInfo __PACK_END;
    __PACK_BEGIN struct {

    } SyncFile __PACK_END;
    __PACK_BEGIN struct {
      uint8_t FileAttr;
    } CreateObj __PACK_END;
  };
  char Filename[FILETRANSFER_FILENAME_LEN];
} __PACK_END FileTransReq_T;

__PACK_BEGIN typedef struct {
  uint32_t PackID;
  uint32_t DataLen;
  uint8_t fData[FILETRANSFER_CACHE_SIZE];
} __PACK_END FileTransData_T;

__PACK_BEGIN typedef struct {
  FileOperateType OptCmd;
  FileOperateState OptSta;
  union {
    __PACK_BEGIN struct {
      uint32_t PackID;
      uint32_t DataLen;
    } PackInfo __PACK_END;
    __PACK_BEGIN struct {}  __PACK_END;
  };
} __PACK_END FileTransAck_T;

#endif /* INC_FILETRANSFER_TYPE_H_ */
