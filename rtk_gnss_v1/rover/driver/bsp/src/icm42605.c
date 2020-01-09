/*
 * icm42605.c
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#include "icm42605.h"
#include "icm426xx_defs.h"

#include <math.h>
#include <string.h>

static const char *TAG = "MEMS";

#define IMU_CACHE_SIZE                           20

#define IMU_GYR_OFFSET_MIN_VAL                   (0.03125f)
#define IMU_GYR_OFFSET_MAX_VAL                   (64.0f)
#define IMU_GYR_OFFSET_RESOLUTION                (0.03125f)

/** register configuration for self-test procedure */
//#define ST_GYRO_FSR             ICM426XX_GYRO_CONFIG0_FS_SEL_250dps
//#define ST_GYRO_ODR             ICM426XX_GYRO_CONFIG0_ODR_1_KHZ
//#define ST_GYRO_UI_FILT_ORD_IND ICM426XX_GYRO_CONFIG_GYRO_UI_FILT_ORD_3RD_ORDER
//#define ST_GYRO_UI_FILT_BW_IND  ICM426XX_GYRO_ACCEL_CONFIG0_GYRO_FILT_BW_10
//
//#define ST_ACCEL_FSR             ICM426XX_ACCEL_CONFIG0_FS_SEL_4g
//#define ST_ACCEL_ODR             ICM426XX_ACCEL_CONFIG0_ODR_1_KHZ
//#define ST_ACCEL_UI_FILT_ORD_IND ICM426XX_ACCEL_CONFIG_ACCEL_UI_FILT_ORD_3RD_ORDER
//#define ST_ACCEL_UI_FILT_BW_IND  ICM426XX_GYRO_ACCEL_CONFIG0_ACCEL_FILT_BW_10

#define ST_GYRO_FSR             ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps
#define ST_GYRO_ODR             ICM426XX_GYRO_CONFIG0_ODR_1_KHZ
#define ST_GYRO_UI_FILT_ORD_IND ICM426XX_GYRO_CONFIG_GYRO_UI_FILT_ORD_3RD_ORDER
#define ST_GYRO_UI_FILT_BW_IND  ICM426XX_GYRO_ACCEL_CONFIG0_GYRO_FILT_BW_10

#define ST_ACCEL_FSR             ICM426XX_ACCEL_CONFIG0_FS_SEL_8g
#define ST_ACCEL_ODR             ICM426XX_ACCEL_CONFIG0_ODR_1_KHZ
#define ST_ACCEL_UI_FILT_ORD_IND ICM426XX_ACCEL_CONFIG_ACCEL_UI_FILT_ORD_3RD_ORDER
#define ST_ACCEL_UI_FILT_BW_IND  ICM426XX_GYRO_ACCEL_CONFIG0_ACCEL_FILT_BW_10

/* Pass/Fail criteria */
#define MIN_ST_GYRO_DPS        60   /* expected values greater than 60dps */
#define MAX_ST_GYRO_OFFSET_DPS 20   /* expected offset less than 20 dps */

#define MIN_RATIO_GYRO  0.5f /* expected ratio greater than 0.5 */
#define MAX_RATIO_GYRO  1.5f /* expected ratio lower than 1.5 */
#define MIN_ST_ACCEL_MG 225  /* expected values in [225mgee;675mgee] */
#define MAX_ST_ACCEL_MG 675

/** collected bias values (lsb) during self test */
int gyro_st_bias[3];
int accel_st_bias[3];

/** @brief Contains the current register values. Used to reapply values after the ST procedure
*/
struct recover_regs {
  /* bank 0 */
  uint8_t intf_config1;       /* REG_INTF_CONFIG1       */
  uint8_t pwr_mgmt_0;         /* REG_PWR_MGMT_0         */
  uint8_t accel_config0;      /* REG_ACCEL_CONFIG0      */
  uint8_t accel_config1;      /* REG_ACCEL_CONFIG1      */
  uint8_t gyro_config0;       /* REG_GYRO_CONFIG0       */
  uint8_t gyro_config1;       /* REG_GYRO_CONFIG1       */
  uint8_t accel_gyro_config0; /* REG_ACCEL_GYRO_CONFIG0 */
  uint8_t fifo_config1;       /* REG_FIFO_CONFIG1       */
  uint8_t self_test_config;   /* REG_SELF_TEST_CONFIG   */
};

struct selftest_vals {
  int32_t st_avg_gyr[3];
  int32_t st_avg_acc[3];
  int32_t st_avg_gyr_st[3];
  int32_t st_avg_acc_st[3];
  uint32_t st_resp_gyr[3];
  uint32_t st_resp_acc[3];
  uint8_t st_code_gyr[3];
  uint8_t st_code_acc[3];
  uint32_t st_STG_OTP[3];
  uint32_t st_STA_OTP[3];
};

/* Formula to get ST_OTP based on FS and ST_code */
#define IMU_ST_OTP_EQUATION(FS, ST_code) (uint32_t)((2620/pow(2,3-FS))*pow(1.01, ST_code-1)+0.5)

static uint32_t icm42605_init_flag = 0;
#if FREERTOS_ENABLED
static osSemaphoreId imu_semaphore;
static uint8_t *imu_tx_buffer = NULL;
static uint8_t *imu_rx_buffer = NULL;
#else
static uint32_t imu_data_ready = 0;
static uint8_t imu_tx_buffer[IMU_CACHE_SIZE] = {0};
static uint8_t imu_rx_buffer[IMU_CACHE_SIZE] = {0};
#endif /* FREERTOS_ENABLED */

#define IMU_ABS(x)                               (((x) > 0) ? (x) : (-(x)))

#define IMU_DRV_CHECK_ASSERT(x) {if((x) != status_ok) return status_error;}

static int reg_to_accel_fsr(ICM426XX_ACCEL_CONFIG0_FS_SEL_t reg);
static int reg_to_gyro_fsr(ICM426XX_GYRO_CONFIG0_FS_SEL_t reg);
static status_t save_settings(struct recover_regs * saved_regs);
static status_t recover_settings(const struct recover_regs * saved_regs);
static status_t set_user_offset_regs(int *accel_st_bias, int *gyro_st_bias);

static status_t imu_read_reg(uint8_t reg, uint8_t num);
static status_t imu_write_reg(uint8_t reg, uint8_t val);

status_t icm42605_init(void)
{
  if(icm42605_init_flag == 1) return status_ok;

#if FREERTOS_ENABLED
  imu_tx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_tx_buffer == NULL) return status_nomem;
  imu_rx_buffer = kmm_alloc(IMU_CACHE_SIZE); if(imu_rx_buffer == NULL) return status_nomem;

  /* Define used semaphore */
  osSemaphoreDef(IMU_SEM);
  /* Create the semaphore */
  imu_semaphore = osSemaphoreCreate(osSemaphore(IMU_SEM) , 1);
  if(imu_semaphore == NULL) return status_error;
#else
  imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

  /* initialize spi to communicate to the icm42605 */
  IMU_DRV_CHECK_ASSERT(imuif_init());

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* software reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SOFT_RESET_ENABLE) );
  /* signal path reset */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_SIGNAL_PATH_RESET, \
                                      IMU_ABORT_AND_RESET | IMU_TMST_STROBE | IMU_FIFO_FLUSH) );
  imu_delay(50); // wait 50ms for reset operation done.
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  IMU_DRV_CHECK_ASSERT(imu_read_reg(REG_WHO_AM_I, 1));
  if(imu_rx_buffer[1] != IMU_ID) {
	  ky_err(TAG, "icm42605 read id failed.");
	  return status_error;
  }

  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* AP interface uses 4-wire SPI mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG4, 0x01 | IMU_SPI_AP_4WIRE_USE_4WIRE) );

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* select spi mode 0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_DEVICE_CONFIG, IMU_SPI_MODE_0_3 | IMU_SOFT_RESET_DISABLE) );
  imu_delay(1);
  /* FIFO mode: Stream-to-FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG, IMU_FIFO_MODE_STREAM) );
  /* INT1 & INT2 Pin configuration */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_CONFIG, \
                                     (IMU_INT_MODE_PULSED | IMU_INT_DRIVE_PP | IMU_INT_POLARITY_HIGH) << IMU_INT2_CFG_BITSHIFT | \
                                     (IMU_INT_MODE_PULSED | IMU_INT_DRIVE_PP | IMU_INT_POLARITY_HIGH) << IMU_INT1_CFG_BITSHIFT));
  /* UI data ready interrupt routed to INT1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE0, IMU_UI_DRDY_INT1_EN) );
  /* UI data ready interrupt routed to INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_SOURCE3, IMU_UI_DRDY_INT2_EN) );
  /* configure watermark of FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG2, 1024 & 0xFF) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG3, (1024 >> 8) & IMU_FIFO_WM_UPPER_BITS_MASK) );
  /* FIFO hold last data, little-endian used */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG0, \
                                      IMU_FIFO_HOLD_LAST_DATA_EN | IMU_FIFO_COUNT_REC_RECORDS | IMU_FIFO_COUNT_ENDIAN_LITTLE | \
                                      IMU_SENSOR_DATA_ENDIAN_LITTLE | IMU_UI_SIFS_CFG_DISABLE_I2C) );
  /* Enable sensor data goes to FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FIFO_CONFIG1, IMU_FIFO_RESUME_PARTIAL_RD | IMU_FIFO_WM_GT_TH | \
                                      IMU_FIFO_TMST_FSYNC_EN | IMU_FIFO_TEMP_EN | IMU_FIFO_GYRO_EN | IMU_FIFO_ACCEL_EN) );
  /* Tag FSYNC flag to TEMP_OUT LSB */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_FSYNC_CONFIG, IMU_FSYNC_UI_SEL_TAG_TEMP_LSB | \
                                      IMU_FSYNC_UI_FLAG_CLEAR_SEL_UPDATE | IMU_FSYNC_POLARITY_RISING) );
  /* User should change setting to 0 from default setting of 1, for proper INT1 and INT2 pin operation */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INT_CONFIG1, IMU_INT_TPULSE_DURATION_100US | IMU_INT_TDEASSERT_DURATION_100US) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_TMST_CONFIG, 0x20 | IMU_TMST_TO_REGS_EN | IMU_TMST_RES_1US | \
                                      IMU_TMST_FSYNC_EN | IMU_TMST_EN) );
  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  /* Enable gyroscope AAF & NF */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG_STATIC2, 0xA8 | IMU_GYRO_AAF_ENABLE | IMU_GYRO_NF_ENABLE) );
  /* set PIN9 as INT2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG5, 0x20 | IMU_PIN9_FUNCTION_INT2) );
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  /* Clock selection */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_INTF_CONFIG1, 0x90 | IMU_ACCEL_LP_CLK_SEL_WKUP_OSC | IMU_CLKSEL_PLL_OR_RC_OSC) );
  /* gyroscope: +/-2000dps@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG0, IMU_GYRO_FS_SEL_2000DPS | IMU_GYRO_ODR_1KHZ) );
  /* accelerator: +/-16g@1KHz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG0, IMU_ACCEL_FS_SEL_8G | IMU_ACCEL_ODR_1KHZ) );
  /* Bandwidth configuration */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_ACCEL_CONFIG0, (IMU_UI_FILT_BW_ODR_10 << IMU_UI_ACCEL_FILT_BW_BITSHIFT) |
                                                              (IMU_UI_FILT_BW_ODR_10 << IMU_UI_GYRO_FILT_BW_BITSHIFT)) );
  /* Selects order of ACCEL UI filter and Order of Accelerometer DEC2_M2 filter */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_ACCEL_CONFIG1, 0x01 | IMU_ACCEL_UI_FILT_ORD_1ST | IMU_ACCEL_DEC2_M2_ORD_3RD) );
  /* temperature: DLPF@4000Hz */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_GYRO_CONFIG1, 0x10 | IMU_TEMP_FILT_BW_4000HZ | IMU_GYRO_UI_FILT_ORD_1ST | IMU_GYRO_DEC2_M2_ORD_3RD) );
  /* Put gyroscope and accelerator sensor in low-noise mode */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_PWR_MGMT0, IMU_GYRO_MODE_LOW_NOISE | IMU_ACCEL_MODE_LOW_NOISE) );

  imu_delay(1);

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  memset(imu_tx_buffer, 0, IMU_CACHE_SIZE);
  memset(imu_rx_buffer, 0, IMU_CACHE_SIZE);

  icm42605_init_flag = 1;
  ky_info(TAG, "icm42605 initialized.");

  return status_ok;
}

status_t icm42605_read(IMU_RAW_6DOF *raw, IMU_UNIT_6DOF *unit, uint32_t timeout)
{
#if FREERTOS_ENABLED
  if(osSemaphoreWait(imu_semaphore , timeout) == osOK) {
#else
  uint32_t st = imu_ticks();
  do {
    if(timeout == 0) {
      break;
    } else if(timeout != HAL_MAX_DELAY) {
      if((HAL_GetTick() - st) > timeout)
        return status_timeout;
    }
  } while(imu_data_ready == 0);
  if(imu_data_ready != 0) {
    imu_data_ready = 0;
#endif /* FREERTOS_ENABLED */

    if(raw != NULL) {
      raw->Acc.X = *((int16_t *)&(imu_rx_buffer[2]));
      raw->Acc.Y = *((int16_t *)&(imu_rx_buffer[4]));
      raw->Acc.Z = *((int16_t *)&(imu_rx_buffer[6]));

      raw->Gyr.X = *((int16_t *)&(imu_rx_buffer[8]));
      raw->Gyr.Y = *((int16_t *)&(imu_rx_buffer[10]));
      raw->Gyr.Z = *((int16_t *)&(imu_rx_buffer[12]));

      raw->Temp = (int8_t)imu_rx_buffer[14];
      raw->TS = *((uint16_t *)&(imu_rx_buffer[15]));
    }

    imu_tx_buffer[0] = REG_FIFO_DATA | 0x80;
    imuif_txrx_bytes_dma(imu_tx_buffer, imu_rx_buffer, 16 +1);

    if(unit != NULL && raw != NULL) {
      unit->Acc.X = raw->Acc.X * 0.002392578125f;
      unit->Acc.Y = raw->Acc.Y * 0.002392578125f;
      unit->Acc.Z = raw->Acc.Z * 0.002392578125f;

      unit->Gyr.X = raw->Gyr.X * 0.06103515625f;
      unit->Gyr.Y = raw->Gyr.Y * 0.06103515625f;
      unit->Gyr.Z = raw->Gyr.Z * 0.06103515625f;

      unit->Temp = raw->Temp / 2.07f + 25.0f;
      unit->TS = raw->TS;
    }

    return status_ok;
  }

  return status_timeout;
}

status_t icm42605_selftest(IMU_RAW_6DOF *imu_raw, uint8_t *result)
{
  uint32_t st_ts = 0, st_tn = 0;

  struct selftest_vals *st_vals = kmm_alloc(sizeof(struct selftest_vals));
  if(st_vals == NULL) return status_nomem;
  memset(st_vals, 0, sizeof(struct selftest_vals));

  struct recover_regs *regs = kmm_alloc(sizeof(struct recover_regs));
  if(regs == NULL) return status_nomem;

  /* Save current settings to restore them at the end of the routine */
  IMU_DRV_CHECK_ASSERT( save_settings(regs) );

  /* Disable Accel and Gyro */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_GYRO_MODE_MASK;
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_ACCEL_MODE_MASK;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_GYRO_MODE_OFF;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_ACCEL_MODE_OFF;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, imu_rx_buffer[1]) );

  /* Set gyro configuration */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_GYRO_CONFIG0, 1) );
  imu_rx_buffer[1] &= ~BIT_GYRO_CONFIG0_FS_SEL_MASK;
  imu_rx_buffer[1] &= ~BIT_GYRO_CONFIG0_ODR_MASK;
  imu_rx_buffer[1] |= ST_GYRO_FSR;
  imu_rx_buffer[1] |= ST_GYRO_ODR;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_GYRO_CONFIG0, imu_rx_buffer[1]) );

  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_GYRO_CONFIG1, 1) );
  imu_rx_buffer[1] &= ~BIT_GYRO_CONFIG1_GYRO_UI_FILT_ORD_MASK;
  imu_rx_buffer[1] |= ST_GYRO_UI_FILT_ORD_IND;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_GYRO_CONFIG1, imu_rx_buffer[1]) );

  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_GYRO_CONFIG0, 1) );
  imu_rx_buffer[1] &= ~BIT_GYRO_ACCEL_CONFIG0_GYRO_FILT_MASK;
  imu_rx_buffer[1] |= ST_GYRO_UI_FILT_BW_IND;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_GYRO_CONFIG0, imu_rx_buffer[1]) );

  /* Set accel configuration */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_CONFIG0, 1) );
  imu_rx_buffer[1] &= ~BIT_ACCEL_CONFIG0_FS_SEL_MASK;
  imu_rx_buffer[1] &= ~BIT_ACCEL_CONFIG0_ODR_MASK;
  imu_rx_buffer[1] |= ST_ACCEL_FSR;
  imu_rx_buffer[1] |= ST_ACCEL_ODR;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_CONFIG0, imu_rx_buffer[1]) );

  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_CONFIG1, 1) );
  imu_rx_buffer[1] &= ~BIT_ACCEL_CONFIG1_ACCEL_UI_FILT_ORD_MASK;
  imu_rx_buffer[1] |= ST_ACCEL_UI_FILT_ORD_IND;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_CONFIG1, imu_rx_buffer[1]) );

  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_GYRO_CONFIG0, 1) );
  imu_rx_buffer[1] &= ~BIT_GYRO_ACCEL_CONFIG0_ACCEL_FILT_MASK;
  imu_rx_buffer[1] |= ST_ACCEL_UI_FILT_BW_IND;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_GYRO_CONFIG0, imu_rx_buffer[1]) );

  /* Read average gyro digital output for each axis and store them as STG_OFF_{x,y,z} in lsb
   * read average accel digital output for each axis and store them as ST_OFF_{x,y,z} in lsb x 1000 */

  /* Enable Gyro and Accel */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_GYRO_MODE_MASK;
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_ACCEL_MODE_MASK;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_GYRO_MODE_LN;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_ACCEL_MODE_LN;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, imu_rx_buffer[1]) );

  /* Flush FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SIGNAL_PATH_RESET, (uint8_t)ICM426XX_SIGNAL_PATH_RESET_FIFO_FLUSH_EN) );

  /* wait for 60ms to allow output to settle */
  imu_delay(60);

  icm42605_read(imu_raw, NULL, osWaitForever);
  int st_samples = 0; /* should be a singed value */
  st_ts = imu_ticks();

  do {
    if(icm42605_read(imu_raw, NULL, 10) == status_ok) {
      if((imu_raw->Gyr.X != -32768) && (imu_raw->Gyr.Y != -32768) && (imu_raw->Gyr.Z != -32768) && \
         (imu_raw->Acc.X != -32768) && (imu_raw->Acc.Y != -32768) && (imu_raw->Acc.Z != -32768)) {
        st_samples ++;
        st_vals->st_avg_gyr[0] += imu_raw->Gyr.X;
        st_vals->st_avg_gyr[1] += imu_raw->Gyr.Y;
        st_vals->st_avg_gyr[2] += imu_raw->Gyr.Z;
        st_vals->st_avg_acc[0] += imu_raw->Acc.X;
        st_vals->st_avg_acc[1] += imu_raw->Acc.Y;
        st_vals->st_avg_acc[2] += imu_raw->Acc.Z;
      }
    }
    st_tn = imu_ticks();
  } while(((st_tn - st_ts) < 300) && (st_samples < 200));

  /* Disable Accel and Gyro */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_GYRO_MODE_MASK;
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_ACCEL_MODE_MASK;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_GYRO_MODE_OFF;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_ACCEL_MODE_OFF;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, imu_rx_buffer[1]) );

  for(int i = 0; i < 3; i ++) {
    st_vals->st_avg_gyr[i] = st_vals->st_avg_gyr[i] / st_samples;
    st_vals->st_avg_acc[i] = st_vals->st_avg_acc[i] / st_samples;
  }

  /* Enable self-test for each axis and read average gyro digital output for each axis and store them as STG_ON_{x,y,z} in lsb */
  /* Enable self-test for each axis and read average gyro digital output for each axis and store them as ST_ON_{x,y,z} in lsb x 1000 */

  /* Enable Gyro and Accel */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_GYRO_MODE_MASK;
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_ACCEL_MODE_MASK;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_GYRO_MODE_LN;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_ACCEL_MODE_LN;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, imu_rx_buffer[1]) );

  /* wait for 60ms to allow output to settle */
  imu_delay(60);

  /* Apply ST config if required */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_SELF_TEST_CONFIG, 1) );
  imu_rx_buffer[1] |= BIT_GYRO_X_ST_EN + BIT_GYRO_Y_ST_EN + BIT_GYRO_Z_ST_EN + \
		              BIT_ACCEL_X_ST_EN + BIT_ACCEL_Y_ST_EN + BIT_ACCEL_Z_ST_EN + BIT_ST_REGULATOR_EN;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SELF_TEST_CONFIG, imu_rx_buffer[1]) );

  /* Flush FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SIGNAL_PATH_RESET, (uint8_t)ICM426XX_SIGNAL_PATH_RESET_FIFO_FLUSH_EN) );
  /* wait 200ms for the oscillation to stabilize */
  imu_delay(200);

  icm42605_read(imu_raw, NULL, osWaitForever);
  st_samples = 0;
  st_ts = imu_ticks();

  do {
    if(icm42605_read(imu_raw, NULL, 10) == status_ok) {
      if((imu_raw->Gyr.X != -32768) && (imu_raw->Gyr.Y != -32768) && (imu_raw->Gyr.Z != -32768) && \
         (imu_raw->Acc.X != -32768) && (imu_raw->Acc.Y != -32768) && (imu_raw->Acc.Z != -32768)) {
        st_samples ++;
        st_vals->st_avg_gyr_st[0] += imu_raw->Gyr.X;
        st_vals->st_avg_gyr_st[1] += imu_raw->Gyr.Y;
        st_vals->st_avg_gyr_st[2] += imu_raw->Gyr.Z;
        st_vals->st_avg_acc_st[0] += imu_raw->Acc.X;
        st_vals->st_avg_acc_st[1] += imu_raw->Acc.Y;
        st_vals->st_avg_acc_st[2] += imu_raw->Acc.Z;
      }
    }
    st_tn = imu_ticks();
  } while(((st_tn - st_ts) < 300) && (st_samples < 200));

  /* Disable Accel and Gyro */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_GYRO_MODE_MASK;
  imu_rx_buffer[1] &= (uint8_t)~BIT_PWR_MGMT_0_ACCEL_MODE_MASK;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_GYRO_MODE_OFF;
  imu_rx_buffer[1] |= (uint8_t)ICM426XX_PWR_MGMT_0_ACCEL_MODE_OFF;
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, imu_rx_buffer[1]) );
  /* Disable self-test config if necessary */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SELF_TEST_CONFIG, 0) );

  for(int i = 0; i < 3; i ++) {
    st_vals->st_avg_gyr_st[i] /= st_samples;
    st_vals->st_avg_acc_st[i] /= st_samples;
  }

  /* calculate the self-test response as ABS(ST_ON_{x,y,z} - ST_OFF_{x,y,z}) for each axis */
  /* outputs from this routine are in units of lsb and hence are dependent on the full-scale used on the DUT */
  for(int i = 0; i < 3; i ++) {
    st_vals->st_resp_gyr[i] = IMU_ABS(st_vals->st_avg_gyr_st[i] - st_vals->st_avg_gyr[i]);
    st_vals->st_resp_acc[i] = IMU_ABS(st_vals->st_avg_acc_st[i] - st_vals->st_avg_acc[i]);
  }

  *result = 0x03;

  /* calculate ST results OTP based on the equation */
  /* select BANK1 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_1) );
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_XG_ST_DATA_B1, 3) );
  /* copy the data immediately */
  for(int i = 0; i < 3; i ++)
    st_vals->st_code_gyr[i] = imu_rx_buffer[1 + i];
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  int fs_sel = ST_GYRO_FSR >> BIT_GYRO_CONFIG0_FS_SEL_POS;
  for (int i = 0; i < 3; i++) {
    st_vals->st_STG_OTP[i] = IMU_ST_OTP_EQUATION(fs_sel, st_vals->st_code_gyr[i]);
  }

  /** Check Gyro self-test results */
  uint32_t gyro_sensitivity_1dps = 32768 / reg_to_gyro_fsr(ST_GYRO_FSR);

  for (int i = 0; i < 3; i++) {
    if (st_vals->st_STG_OTP[i]) {
      float ratio = ((float)st_vals->st_resp_gyr[i]) / ((float)st_vals->st_STG_OTP[i]);
      if (ratio <= MIN_RATIO_GYRO)
        *result &= 0xFE; /* fail */
    } else if(st_vals->st_resp_gyr[i] < (MIN_ST_GYRO_DPS * gyro_sensitivity_1dps)) {
      *result &= 0xFE; /* fail */
    }
  }

  /* stored the computed bias (checking GST and GOFFSET values) */
  for (int i = 0; i < 3; i++) {
    if((IMU_ABS(st_vals->st_avg_gyr[i]) > (int32_t)(MAX_ST_GYRO_OFFSET_DPS * gyro_sensitivity_1dps)))
      *result &= 0xFE; /* fail */
    gyro_st_bias[i] = st_vals->st_avg_gyr[i];
  }

  /* select BANK2 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_2) );
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_XA_ST_DATA_B2, 3) );
  /* copy the data immediately */
  for(int i = 0; i < 3; i ++)
    st_vals->st_code_acc[i] = imu_rx_buffer[1 + i];
  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  fs_sel = ST_ACCEL_FSR >> BIT_ACCEL_CONFIG0_FS_SEL_POS;
  for (int i = 0; i < 3; i++) {
    st_vals->st_STA_OTP[i] = IMU_ST_OTP_EQUATION(fs_sel, st_vals->st_code_acc[i]);
  }

  /** Check Accel self-test result */
  uint32_t accel_sensitivity_1g = 32768 / reg_to_accel_fsr(ST_ACCEL_FSR);

  for (int i = 0; i < 3; i++) {
    if (st_vals->st_STA_OTP[i]) {
      float ratio = ((float)st_vals->st_resp_acc[i]) / ((float)st_vals->st_STA_OTP[i]);
      if ((ratio >= MAX_RATIO_GYRO) || (ratio <= MIN_RATIO_GYRO))
        *result &= 0xFD; /* fail */
    } else if ((st_vals->st_resp_acc[i] < ((MIN_ST_ACCEL_MG * accel_sensitivity_1g) / 1000))
            || (st_vals->st_resp_acc[i] > ((MAX_ST_ACCEL_MG * accel_sensitivity_1g) / 1000))) {
      *result &= 0xFD; /* fail */
    }
  }

  /* stored the computed offset */
  for(int i = 0; i < 3; i++) {
    accel_st_bias[i] = st_vals->st_avg_acc[i];
  }

  /* assume the largest data axis shows +1 or -1 gee for gravity */
  int axis = 0;
  int axis_sign = 1;
  if (IMU_ABS(accel_st_bias[1]) > IMU_ABS(accel_st_bias[0]))
    axis = 1;
  if (IMU_ABS(accel_st_bias[2]) > IMU_ABS(accel_st_bias[axis]))
    axis = 2;
  if (accel_st_bias[axis] < 0)
    axis_sign = -1;

  uint32_t gravity = accel_sensitivity_1g * axis_sign;
  accel_st_bias[axis] -= gravity;

  IMU_DRV_CHECK_ASSERT( set_user_offset_regs(accel_st_bias, gyro_st_bias) );

  IMU_DRV_CHECK_ASSERT( recover_settings(regs) );
  kmm_free(regs);
  kmm_free(st_vals);
  return status_ok;
}

static int reg_to_accel_fsr(ICM426XX_ACCEL_CONFIG0_FS_SEL_t reg)
{
  switch(reg) {
    case ICM426XX_ACCEL_CONFIG0_FS_SEL_2g:   return 2;
    case ICM426XX_ACCEL_CONFIG0_FS_SEL_4g:   return 4;
    case ICM426XX_ACCEL_CONFIG0_FS_SEL_8g:   return 8;
    case ICM426XX_ACCEL_CONFIG0_FS_SEL_16g:  return 16;
    default:                                 return -1;
  }
}

static int reg_to_gyro_fsr(ICM426XX_GYRO_CONFIG0_FS_SEL_t reg)
{
  switch(reg) {
    case ICM426XX_GYRO_CONFIG0_FS_SEL_31dps:   return 31;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_62dps:   return 62;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_125dps:  return 125;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_250dps:  return 250;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_500dps:  return 500;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_1000dps: return 1000;
    case ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps: return 2000;
    default:                                   return -1;
  }
}

static status_t save_settings(struct recover_regs * saved_regs)
{
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_INTF_CONFIG1, 1) );
  saved_regs->intf_config1 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_PWR_MGMT_0, 1) );
  saved_regs->pwr_mgmt_0 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_CONFIG0, 1) );
  saved_regs->accel_config0 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_CONFIG1, 1) );
  saved_regs->accel_config1 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_GYRO_CONFIG0, 1) );
  saved_regs->gyro_config0 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_GYRO_CONFIG1, 1) );
  saved_regs->gyro_config1 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_ACCEL_GYRO_CONFIG0, 1) );
  saved_regs->accel_gyro_config0 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_FIFO_CONFIG1, 1) );
  saved_regs->fifo_config1 = imu_rx_buffer[1];
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_SELF_TEST_CONFIG, 1) );
  saved_regs->self_test_config = imu_rx_buffer[1];

  return status_ok;
}

static status_t recover_settings(const struct recover_regs * saved_regs)
{
  /* Set en_g{x/y/z}_st_d2a to 0 disable self-test for each axis */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SELF_TEST_CONFIG, saved_regs->self_test_config) );
  /*Restore gyro_dec2_m2_ord, gyro_ui_filt_ord_ind and gyro_ui_filt_bw_ind to previous values.*/
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_INTF_CONFIG1, saved_regs->intf_config1) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_PWR_MGMT_0, saved_regs->pwr_mgmt_0) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_CONFIG0, saved_regs->accel_config0) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_CONFIG1, saved_regs->accel_config1) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_GYRO_CONFIG0, saved_regs->gyro_config0) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_GYRO_CONFIG1, saved_regs->gyro_config1) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_FIFO_CONFIG1, saved_regs->fifo_config1) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_ACCEL_GYRO_CONFIG0, saved_regs->accel_gyro_config0) );
  /* wait 200ms for gyro output to settle */
  imu_delay(200);

  /* Flush FIFO */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(MPUREG_SIGNAL_PATH_RESET, (uint8_t)ICM426XX_SIGNAL_PATH_RESET_FIFO_FLUSH_EN) );
  /* wait for 60ms to allow output to settle */
  imu_delay(60);

  return status_ok;
}

#define ACCEL_CONFIG0_FS_SEL_MAX ICM426XX_ACCEL_CONFIG0_FS_SEL_16g
#define GYRO_CONFIG0_FS_SEL_MAX  ICM426XX_GYRO_CONFIG0_FS_SEL_2000dps

#define ACCEL_OFFUSER_MAX_MG 1000
#define GYRO_OFFUSER_MAX_DPS 64

static status_t set_user_offset_regs(int *accel_st_bias, int *gyro_st_bias)
{
  uint8_t data[5];
  int16_t cur_bias;

  // Set memory bank 4
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_4) );

  /* Set offset registers sensor */

  /*
   * Invert sign for OFFSET and
   * accel_st_bias is 2g coded 16
   * OFFUSER is 1g coded 12 (or 2g coded 12 for High FSR parts)
   */

  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_OFFSET_USER_4_B4, 1) ); // Fetch gyro_z_offuser[8-11]
  imu_rx_buffer[1] &= BIT_GYRO_Z_OFFUSER_MASK_HI;
  data[0] = imu_rx_buffer[1];

  cur_bias = (int16_t)(-accel_st_bias[0] >> 3);
  cur_bias /= ACCEL_OFFUSER_MAX_MG / 1000;
  data[0] |= (((cur_bias & 0x0F00) >> 8) << BIT_ACCEL_X_OFFUSER_POS_HI);
  data[1] = ((cur_bias & 0x00FF) << BIT_ACCEL_X_OFFUSER_POS_LO);
  cur_bias = (int16_t)(-accel_st_bias[1] >> 3);
  cur_bias /= ACCEL_OFFUSER_MAX_MG / 1000;
  data[2] = ((cur_bias & 0x00FF) << BIT_ACCEL_Y_OFFUSER_POS_LO);
  data[3] = (((cur_bias & 0x0F00) >> 8) << BIT_ACCEL_Y_OFFUSER_POS_HI);
  cur_bias = (int16_t)(-accel_st_bias[2] >> 3);
  cur_bias /= ACCEL_OFFUSER_MAX_MG / 1000;
  data[3] |= (((cur_bias & 0x0F00) >> 8) << BIT_ACCEL_Z_OFFUSER_POS_HI);
  data[4] = ((cur_bias & 0x00FF) << BIT_ACCEL_Z_OFFUSER_POS_LO);

  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER4, data[0]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER5, data[1]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER6, data[2]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER7, data[3]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER8, data[4]) );

  /*
   * Invert sign for OFFSET and
   * gyro_st_bias is 250dps coded 16
   * OFFUSER is 64dps coded 12 (or 128dps coded 12 for High FSR parts)
   */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(MPUREG_OFFSET_USER_4_B4, 1) ); // Fetch accel_x_offuser[8-11]
  imu_rx_buffer[1] &= BIT_ACCEL_X_OFFUSER_MASK_HI;
  data[4] = imu_rx_buffer[1];

  cur_bias = (int16_t)(-(gyro_st_bias[0] * 250 / GYRO_OFFUSER_MAX_DPS) >> 4);
  data[0] = ((cur_bias & 0x00FF) << BIT_GYRO_X_OFFUSER_POS_LO);
  data[1] = (((cur_bias & 0x0F00) >> 8) << BIT_GYRO_X_OFFUSER_POS_HI);
  cur_bias = (int16_t)(-(gyro_st_bias[1] * 250 / GYRO_OFFUSER_MAX_DPS) >> 4);
  data[1] |= (((cur_bias & 0x0F00) >> 8) << BIT_GYRO_Y_OFFUSER_POS_HI);
  data[2] = ((cur_bias & 0x00FF) << BIT_GYRO_Y_OFFUSER_POS_LO);
  cur_bias = (int16_t)(-(gyro_st_bias[2] * 250 / GYRO_OFFUSER_MAX_DPS) >> 4);
  data[3] = ((cur_bias & 0x00FF) << BIT_GYRO_Z_OFFUSER_POS_LO);
  data[4] |= (((cur_bias & 0x0F00) >> 8) << BIT_GYRO_Z_OFFUSER_POS_HI);

  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER0, data[0]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER1, data[1]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER2, data[2]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER3, data[3]) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER4, data[4]) );

  // Set memory bank 0
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );

  return status_ok;
}

status_t icm42605_gyr_offset(_3AxisUnit *gyr_off)
{
  int offx = 0, offy = 0, offz = 0, org = 0;

  /* select BANK4 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_4) );

  /* get origin offset */
  IMU_DRV_CHECK_ASSERT( imu_read_reg(REG_OFFSET_USER0, 5) );

  /* GYRO_X_OFFSET */
  org = (((uint16_t)imu_rx_buffer[2] & 0x07) << 8) | imu_rx_buffer[1];
  if(imu_rx_buffer[2] & 0x08) org = -org;
  offx = org - (int)gyr_off->X / IMU_GYR_OFFSET_RESOLUTION;
  if(offx < 0) offx |= 0x08000;

  /* GYRO_Y_OFFSET */
  org = ((((uint16_t)imu_rx_buffer[2] & 0x70) >> 4) << 8) | imu_rx_buffer[3];
  if(imu_rx_buffer[2] & 0x80) org = -org;
  offy = org - (int)gyr_off->Y / IMU_GYR_OFFSET_RESOLUTION;
  if(offy < 0) offy |= 0x08000;

  /* GYRO_Z_OFFSET */
  org = (((uint16_t)imu_rx_buffer[5] & 0x07) << 8) | imu_rx_buffer[4];
  if(imu_rx_buffer[5] & 0x08) org = -org;
  offz = org - (int)gyr_off->Z / IMU_GYR_OFFSET_RESOLUTION;
  if(offz < 0) offz |= 0x08000;

  /* save ACCEL_X_OFFUSER[11:8] */
  org = imu_rx_buffer[5];

  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER0, (offx & 0xFF)) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER1, ((offx >> 8) & 0x0F) | ((offy >> 4) & 0xF0)) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER2, (offy & 0xFF)) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER3, (offz & 0xFF)) );
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_OFFSET_USER4, ((offz >> 8) & 0x0F) | (org & 0x00F0)) );

  /* select BANK0 */
  IMU_DRV_CHECK_ASSERT( imu_write_reg(REG_REG_BANK_SEL, IMU_BANK_SEL_0) );
  return status_ok;
}

void imuif_int1_callback(void)
{
  if(icm42605_init_flag != 0) {
#if FREERTOS_ENABLED
    osSemaphoreRelease(imu_semaphore);
#else
    imu_data_ready = 1;
#endif /* FREERTOS_ENABLED */
  }
}

void imuif_int2_callback(void)
{}

static status_t imu_read_reg(uint8_t reg, uint8_t num)
{
  status_t ret = status_error;
  if(num > IMU_CACHE_SIZE - 1) return ret;
  imu_tx_buffer[0] = reg | 0x80;
  ret = imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, num +1);
  return ret;
}

static status_t imu_write_reg(uint8_t reg, uint8_t val)
{
  status_t ret = status_error;
  imu_tx_buffer[0] = reg;
  imu_tx_buffer[1] = val;
  ret = imuif_txrx_bytes(imu_tx_buffer, imu_rx_buffer, 2);
  return ret;
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
