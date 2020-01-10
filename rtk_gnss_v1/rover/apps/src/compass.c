/*
 * compass.c
 *
 *  Created on: Jan 8, 2020
 *      Author: kychu
 */

#include "compass.h"

#include "mesg.h"
#include "ff_gen_drv.h"

static const char *TAG = "MAG";

#define COMPASS_SENSOR_NUMBER                    (2)

#define COMPASS_CALIB_DATA_PATH                  "0:/calib/mag.dat"
#define COS_PI_4                                 (0.70710678118655f)

static const uint8_t IST8310_DEV_ADDR[COMPASS_SENSOR_NUMBER] = { 0x1A, 0x1C };
static const uint8_t CALIB_DATA_PATH[] = COMPASS_CALIB_DATA_PATH;

static struct ElipCalibData {
  float offX, offY, offZ;
  float sclX, sclY, sclZ;
} *calib_data = NULL;

__PACK_BEGIN typedef struct {
  uint8_t id;
  _3AxisRaw raw;
} __PACK_END MagRawDef;

static uint32_t mag_calibrated = 0;
static uint32_t mag_interference = 0;

static Vector3D mag_data;
static osMutexId mag_mutex;

static uint32_t msg_mag_ts = 0; // timestamp for this message
//                      default:      id       disabled  10Hz    parameters
static struct MsgInfo msg_mag = { MAG_DATA_MSG,    0,     10,   {0, 0, 0, 0} };
static uint32_t msg_mag_org_ts = 0; // timestamp for this message
//                      default:           id          disabled  10Hz  start id    number of sensors
static struct MsgInfo msg_mag_org = { MAG_ORG_DATA_MSG,    0,     10,     {0,    COMPASS_SENSOR_NUMBER, 0, 0} };

static struct MsgList msg_mag_org_list = { &msg_mag_org, NULL };
static struct MsgList msg_list = { &msg_mag, &msg_mag_org_list };

static int read_calibrate_data(void);
static int decode_calib_file(FIL *fp);

void magnetics_task(void const *argument)
{
  uint32_t time_now;
#if (COMPASS_SENSOR_NUMBER > 1)
  float cos_alpha;
#endif /* (COMPASS_SENSOR_NUMBER > 1) */
  Vector3D *ist_val = kmm_alloc(COMPASS_SENSOR_NUMBER * sizeof(Vector3D));
  MagRawDef *ist_raw = kmm_alloc(COMPASS_SENSOR_NUMBER * sizeof(MagRawDef));
  calib_data = kmm_alloc(COMPASS_SENSOR_NUMBER * sizeof(struct ElipCalibData));
  ist83xx_dev_t *ist_dev = kmm_alloc(COMPASS_SENSOR_NUMBER * sizeof(ist83xx_dev_t));
  if((ist_val == NULL) || \
     (ist_raw == NULL) || \
     (ist_dev == NULL) || \
     (calib_data == NULL)) {
	ky_err(TAG, "ist8310 memory alloc failed. EXIT!");
    goto error_exit;
  }

  osMutexDef(MAG_DATA_MUTEX);
  mag_mutex = osMutexCreate(osMutex(MAG_DATA_MUTEX));
  if(mag_mutex == NULL) {
    ky_err(TAG, "mag data mutex create failed.");
    goto error_exit;
  }

  ky_info(TAG, "init magnetics sensor.");
  for(int i = 0; i < COMPASS_SENSOR_NUMBER; i ++) {
    ist_dev[i].dev_addr = IST8310_DEV_ADDR[i];
    ist_dev[i].io_init = magif_init;
    ist_dev[i].io_ready = magif_check_ready;
    ist_dev[i].read_reg = magif_read_mem_dma;
    ist_dev[i].write_reg = magif_write_mem_dma;
    if(ist83xx_init(&ist_dev[i]) != status_ok) {
      ky_err(TAG, "ist8310 %d init failed.", i);
      goto error_exit;
    }
    ist_raw[i].id = i;
  }

  if(read_calibrate_data() == 0) mag_calibrated = 1;

  mesg_publish_mesg(&msg_list);

  for(;;) {
    osDelay(5); // update every 5ms.
    for(int i = 0; i < COMPASS_SENSOR_NUMBER; i ++) {
      ist83xx_read_data(&ist_dev[i], &(ist_raw[i].raw));
      if(mag_calibrated) {
        ist_val[i].X = (ist_raw[i].raw.X + calib_data[i].offX) * calib_data[i].sclX;
        ist_val[i].Y = (ist_raw[i].raw.Y + calib_data[i].offY) * calib_data[i].sclY;
        ist_val[i].Z = (ist_raw[i].raw.Z + calib_data[i].offZ) * calib_data[i].sclZ;
        NormalizeVector(&ist_val[i]);
      }
    }
    if(mag_calibrated) {
#if (COMPASS_SENSOR_NUMBER > 1)
      cos_alpha = ScalarProduct(&ist_val[0], &ist_val[1]);
      if(cos_alpha > COS_PI_4) {
        mag_interference = 1;
      } else {
        mag_interference = 0;
      }
#endif /* (COMPASS_SENSOR_NUMBER > 1) */
      osMutexWait(mag_mutex, osWaitForever);
#if (COMPASS_SENSOR_NUMBER > 1)
      mag_data.X = 0;
      mag_data.Y = 0;
      mag_data.Z = 0;
      for(int i = 0; i < COMPASS_SENSOR_NUMBER; i ++) {
        mag_data.X += ist_val[i].X;
        mag_data.Y += ist_val[i].Y;
        mag_data.Z += ist_val[i].Z;
      }
      mag_data.X /= COMPASS_SENSOR_NUMBER;
      mag_data.Y /= COMPASS_SENSOR_NUMBER;
      mag_data.Z /= COMPASS_SENSOR_NUMBER;
      NormalizeVector(&mag_data);
#else
      mag_data.X = ist_val[0].X;
      mag_data.Y = ist_val[0].Y;
      mag_data.Z = ist_val[0].Z;
#endif /* (COMPASS_SENSOR_NUMBER > 1) */
      osMutexRelease(mag_mutex);
    }

    /* update message */
    time_now = xTaskGetTickCountFromISR();

    /* send normalized data */
    if(msg_mag.msg_st & 0x01) {
      if((time_now - msg_mag_ts) >= 1000 / msg_mag.msg_rt) {
        msg_mag_ts = time_now;
        mesg_send_mesg(&mag_data, msg_mag.msg_id, sizeof(Vector3D));
      }
    }
    /* send origin data */
    if(msg_mag_org.msg_st & 0x01) {
      if((time_now - msg_mag_org_ts) > 1000 / msg_mag_org.msg_rt) {
        msg_mag_org_ts = time_now;
        if(msg_mag_org.msg_pr[0] < COMPASS_SENSOR_NUMBER) {
          if(msg_mag_org.msg_pr[1] > (COMPASS_SENSOR_NUMBER - msg_mag_org.msg_pr[0]))
            msg_mag_org.msg_pr[1] = COMPASS_SENSOR_NUMBER - msg_mag_org.msg_pr[0];
          mesg_send_mesg(&ist_raw[msg_mag_org.msg_pr[0]], msg_mag_org.msg_id, msg_mag_org.msg_pr[0] * sizeof(_3AxisRaw));
        }
      }
    }
  }

error_exit:
  if(ist_val != NULL)
    kmm_free(ist_val);
  if(ist_raw != NULL)
    kmm_free(ist_raw);
  if(ist_dev != NULL)
    kmm_free(ist_dev);
  if(calib_data != NULL)
    kmm_free(calib_data);
  if(mag_mutex != NULL)
    osMutexDelete(mag_mutex);
  vTaskDelete(NULL);
}

static int read_calibrate_data(void)
{
  FRESULT ret;
  FIL *file;
  file = kmm_alloc(sizeof(FIL));
  if(file == NULL) {
    ky_err(TAG, "no mem to open calib file.");
    goto error;
  }

  ret = f_open(file, (const TCHAR *)CALIB_DATA_PATH, FA_READ);
  if(ret != FR_OK) {
    ky_err(TAG, "open %s failed. -%d", CALIB_DATA_PATH, ret);
    goto error;
  } else {
    if(decode_calib_file(file) != 0) {
      goto error;
    }
    ret = f_close(file);
    if(ret != FR_OK) {
      ky_err(TAG, "calib file close failed. -%d", ret);
      goto error;
    }
  }

  return 0;
error:
  if(file != NULL)
    kmm_free(file);
  return -1;
}

static int decode_calib_file(FIL *fp)
{
  int i, j;
  FRESULT ret;
  uint8_t *rtext;
  UINT bytes_rd;
  uint8_t checksum;
  rtext = kmm_alloc(sizeof(struct ElipCalibData) + 1);
  if(rtext == NULL) {
    ky_err(TAG, "rtext alloc fail.");
    goto error;
  }
  memset(rtext, 0, sizeof(struct ElipCalibData) + 1);
  // get file header <5B> and number of calibration data <1B>.
  ret = f_read(fp, rtext, 6, (UINT*)&bytes_rd);
  if(ret != FR_OK || bytes_rd < 6) {
    ky_err(TAG, "read calib data error! -%d", ret);
    goto error;
  }
  if(strcmp(".mcd\n", (const char *)rtext) != 0) {
    ky_err(TAG, "file header error!");
    goto error;
  }
  if(rtext[5] != COMPASS_SENSOR_NUMBER) {
    ky_err(TAG, "calib number error! -%d", rtext[5]);
    goto error;
  }
  for(i = 0; i < COMPASS_SENSOR_NUMBER; i ++) {
    ret = f_read(fp, rtext, sizeof(struct ElipCalibData) + 1, (UINT*)&bytes_rd);
    if(ret != FR_OK || bytes_rd < sizeof(struct ElipCalibData) + 1) {
      ky_err(TAG, "read data<%d> failed.", i);
      goto error;
    }
    checksum = rtext[1];
    for(j = 1; j < sizeof(struct ElipCalibData); j ++) checksum ^= rtext[j + 1];
    if(checksum != rtext[0]) {
      ky_err(TAG, "checksum error! %d: %d, %d", i, rtext[0], checksum);
      goto error;
    }
  }
  return 0;

error:
  if(rtext != NULL)
    kmm_free(rtext);
  return -1;
}

int magnetics_take(Vector3D *data)
{
  if(mag_mutex == NULL) return -1;
  if(mag_calibrated == 0) return -1;
  if(mag_interference) return -1;

  osMutexWait(mag_mutex, osWaitForever);
  *data = mag_data;
  osMutexRelease(mag_mutex);
  return 0;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
