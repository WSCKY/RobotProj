/*
 * icm42605.h
 *
 *  Created on: Dec 3, 2019
 *      Author: kychu
 */

#ifndef DRIVER_BSP_INC_ICM42605_H_
#define DRIVER_BSP_INC_ICM42605_H_

#include "drivers.h"

#if FREERTOS_ENABLED
#define imu_delay                                osDelay
#define imu_ticks                                xTaskGetTickCountFromISR
#else
#define imu_delay                                HAL_Delay
#define imu_ticks                                HAL_GetTick
#endif /* FREERTOS_ENABLE */

/* --------------------------- BANK0 --------------------------- */
#define REG_DEVICE_CONFIG                        0x11
#define IMU_SPI_MODE_0_3                         (0x0 << 4)
#define IMU_SPI_MODE_1_2                         (0x1 << 4)
#define IMU_SOFT_RESET_ENABLE                    (0x1 << 0)
#define IMU_SOFT_RESET_DISABLE                   (0x0 << 0)

#define REG_DRIVE_CONFIG                         0x13
#define IMU_I2C_SLEW_RATE_BITSHIFT               3
#define IMU_SPI_SLEW_RATE_BITSHIFT               0
#define IMU_SLEW_RATE_20_60NS                    (0x0)
#define IMU_SLEW_RATE_12_36NS                    (0x1)
#define IMU_SLEW_RATE_6_18NS                     (0x2)
#define IMU_SLEW_RATE_4_12NS                     (0x3)
#define IMU_SLEW_RATE_2_6NS                      (0x4)
#define IMU_SLEW_RATE_LESS_2NS                   (0x5)

#define REG_INT_CONFIG                           0x14
#define IMU_INT2_CFG_BITSHIFT                    3
#define IMU_INT1_CFG_BITSHIFT                    0
#define IMU_INT_MODE_PULSED                      (0x0 << 2)
#define IMU_INT_MODE_LATCHED                     (0x1 << 2)
#define IMU_INT_DRIVE_OD                         (0x0 << 1)
#define IMU_INT_DRIVE_PP                         (0x1 << 1)
#define IMU_INT_POLARITY_LOW                     (0x0 << 0)
#define IMU_INT_POLARITY_HIGH                    (0x1 << 0)

#define REG_FIFO_CONFIG                          0x16
#define IMU_FIFO_MODE_BYPASS                     (0x0 << 6)
#define IMU_FIFO_MODE_STREAM                     (0x1 << 6)
#define IMU_FIFO_MODE_STOP_ON_FULL               (0x2 << 6)

#define REG_TEMP_DATA1                           0x1D
#define REG_TEMP_DATA0                           0x1E
#define REG_ACCEL_DATA_X1                        0x1F
#define REG_ACCEL_DATA_X0                        0x20
#define REG_ACCEL_DATA_Y1                        0x21
#define REG_ACCEL_DATA_Y0                        0x22
#define REG_ACCEL_DATA_Z1                        0x23
#define REG_ACCEL_DATA_Z0                        0x24
#define REG_GYRO_DATA_X1                         0x25
#define REG_GYRO_DATA_X0                         0x26
#define REG_GYRO_DATA_Y1                         0x27
#define REG_GYRO_DATA_Y0                         0x28
#define REG_GYRO_DATA_Z1                         0x29
#define REG_GYRO_DATA_Z0                         0x2A
#define REG_TMST_FSYNCH                          0x2B
#define REG_TMST_FSYNCL                          0x2C

#define REG_INT_STATUS                           0x2D
#define IMU_UI_FSYNC_INT_BIT                     0x40
#define IMU_PLL_RDY_INT_BIT                      0x20
#define IMU_RESET_DONE_INT_BIT                   0x10
#define IMU_DATA_RDY_INT_BIT                     0x08
#define IMU_FIFO_THS_INT_BIT                     0x04
#define IMU_FIFO_FULL_INT_BIT                    0x02
#define IMU_AGC_RDY_INT_BIT                      0x01

#define REG_FIFO_COUNTH                          0x2E
#define REG_FIFO_COUNTL                          0x2F
#define REG_FIFO_DATA                            0x30

#define REG_SIGNAL_PATH_RESET                    0x4B
#define IMU_DMP_INIT_EN                          (0x1 << 6)
#define IMU_DMP_MEM_RESET_EN                     (0x1 << 5)
#define IMU_ABORT_AND_RESET                      (0x1 << 3)
#define IMU_TMST_STROBE                          (0x1 << 2)
#define IMU_FIFO_FLUSH                           (0x1 << 1)

#define REG_INTF_CONFIG0                         0x4C
#define IMU_FIFO_HOLD_LAST_DATA_EN               (0x1 << 7)
#define IMU_FIFO_COUNT_REC_BYTES                 (0x0 << 6)
#define IMU_FIFO_COUNT_REC_RECORDS               (0x1 << 6)
#define IMU_FIFO_COUNT_ENDIAN_LITTLE             (0x0 << 5)
#define IMU_FIFO_COUNT_ENDIAN_BIG                (0x1 << 5)
#define IMU_SENSOR_DATA_ENDIAN_LITTLE            (0x0 << 4)
#define IMU_SENSOR_DATA_ENDIAN_BIG               (0x1 << 4)
#define IMU_UI_SIFS_CFG_DISABLE_SPI              (0x2 << 0)
#define IMU_UI_SIFS_CFG_DISABLE_I2C              (0x3 << 0)

#define REG_INTF_CONFIG1                         0x4D
#define IMU_ACCEL_LP_CLK_SEL_WKUP_OSC            (0x0 << 3)
#define IMU_ACCEL_LP_CLK_SEL_RC_OSC              (0x1 << 3)
#define IMU_CLKSEL_INTERNAL_RC_OSC               (0x0 << 0)
#define IMU_CLKSEL_PLL_OR_RC_OSC                 (0x1 << 0)
#define IMU_CLKSEL_DISABLE_ALL_CLOCK             (0x3 << 0)

#define REG_PWR_MGMT0                            0x4E
#define IMU_TEMP_DIS                             (0x1 << 5)
#define IMU_IDLE                                 (0x1 << 4)
#define IMU_GYRO_MODE_OFF                        (0x0 << 2)
#define IMU_GYRO_MODE_STANDBY                    (0x1 << 2)
#define IMU_GYRO_MODE_LOW_NOISE                  (0x3 << 2)
#define IMU_ACCEL_MODE_OFF                       (0x0 << 0)
#define IMU_ACCEL_MODE_LOW_POWER                 (0x2 << 0)
#define IMU_ACCEL_MODE_LOW_NOISE                 (0x3 << 0)

#define REG_GYRO_CONFIG0                         0x4F
#define IMU_GYRO_FS_SEL_2000DPS                  (0x0 << 5)
#define IMU_GYRO_FS_SEL_1000DPS                  (0x1 << 5)
#define IMU_GYRO_FS_SEL_500DPS                   (0x2 << 5)
#define IMU_GYRO_FS_SEL_250DPS                   (0x3 << 5)
#define IMU_GYRO_FS_SEL_125DPS                   (0x4 << 5)
#define IMU_GYRO_FS_SEL_62P5DPS                  (0x5 << 5)
#define IMU_GYRO_FS_SEL_31P25DPS                 (0x6 << 5)
#define IMU_GYRO_FS_SEL_15P625DPS                (0x7 << 5)
#define IMU_GYRO_ODR_8KHZ                        (0x3 << 0)
#define IMU_GYRO_ODR_4KHZ                        (0x4 << 0)
#define IMU_GYRO_ODR_2KHZ                        (0x5 << 0)
#define IMU_GYRO_ODR_1KHZ                        (0x6 << 0)
#define IMU_GYRO_ODR_200HZ                       (0x7 << 0)
#define IMU_GYRO_ODR_100HZ                       (0x8 << 0)
#define IMU_GYRO_ODR_50HZ                        (0x9 << 0)
#define IMU_GYRO_ODR_25HZ                        (0xA << 0)
#define IMU_GYRO_ODR_12P5HZ                      (0xB << 0)
#define IMU_GYRO_ODR_500HZ                       (0xF << 0)

#define REG_ACCEL_CONFIG0                        0x50
#define IMU_ACCEL_FS_SEL_16G                     (0x0 << 5)
#define IMU_ACCEL_FS_SEL_8G                      (0x1 << 5)
#define IMU_ACCEL_FS_SEL_4G                      (0x2 << 5)
#define IMU_ACCEL_FS_SEL_2G                      (0x3 << 5)
#define IMU_ACCEL_ODR_8KHZ                       (0x3 << 0)
#define IMU_ACCEL_ODR_4KHZ                       (0x4 << 0)
#define IMU_ACCEL_ODR_2KHZ                       (0x5 << 0)
#define IMU_ACCEL_ODR_1KHZ                       (0x6 << 0)
#define IMU_ACCEL_ODR_200HZ                      (0x7 << 0)
#define IMU_ACCEL_ODR_100HZ                      (0x8 << 0)
#define IMU_ACCEL_ODR_50HZ                       (0x9 << 0)
#define IMU_ACCEL_ODR_25HZ                       (0xA << 0)
#define IMU_ACCEL_ODR_12P5HZ                     (0xB << 0)
#define IMU_ACCEL_ODR_6P25HZ                     (0xC << 0)
#define IMU_ACCEL_ODR_3P125HZ                    (0xD << 0)
#define IMU_ACCEL_ODR_1P5625HZ                   (0xE << 0)
#define IMU_ACCEL_ODR_500HZ                      (0xF << 0)

#define REG_GYRO_CONFIG1                         0x51
#define IMU_TEMP_FILT_BW_4000HZ                  (0x0 << 5)
#define IMU_TEMP_FILT_BW_170HZ                   (0x1 << 5)
#define IMU_TEMP_FILT_BW_82HZ                    (0x2 << 5)
#define IMU_TEMP_FILT_BW_40HZ                    (0x3 << 5)
#define IMU_TEMP_FILT_BW_20HZ                    (0x4 << 5)
#define IMU_TEMP_FILT_BW_10HZ                    (0x5 << 5)
#define IMU_TEMP_FILT_BW_5HZ                     (0x6 << 5)
#define IMU_GYRO_UI_FILT_ORD_1ST                 (0x0 << 2)
#define IMU_GYRO_UI_FILT_ORD_2ND                 (0x1 << 2)
#define IMU_GYRO_UI_FILT_ORD_3RD                 (0x2 << 2)
#define IMU_GYRO_DEC2_M2_ORD_3RD                 (0x2 << 0)

#define REG_GYRO_ACCEL_CONFIG0                   0x52
#define IMU_UI_ACCEL_FILT_BW_BITSHIFT            4
#define IMU_UI_GYRO_FILT_BW_BITSHIFT             0
#define IMU_UI_FILT_BW_ODR_2                     (0x0)
#define IMU_UI_FILT_BW_ODR_4                     (0x1)
#define IMU_UI_FILT_BW_ODR_5                     (0x2)
#define IMU_UI_FILT_BW_ODR_8                     (0x3)
#define IMU_UI_FILT_BW_ODR_10                    (0x4)
#define IMU_UI_FILT_BW_ODR_16                    (0x5)
#define IMU_UI_FILT_BW_ODR_20                    (0x6)
#define IMU_UI_FILT_BW_ODR_40                    (0x7)
#define IMU_UI_FILT_DEC2_ODR_400HZ               (0xE)
#define IMU_UI_FILT_DEC2_8ODR_200HZ              (0xF)

#define REG_ACCEL_CONFIG1                        0x53
#define IMU_ACCEL_UI_FILT_ORD_1ST                (0x0 << 3)
#define IMU_ACCEL_UI_FILT_ORD_2ND                (0x1 << 3)
#define IMU_ACCEL_UI_FILT_ORD_3RD                (0x2 << 3)
#define IMU_ACCEL_DEC2_M2_ORD_3RD                (0x2 << 1)

#define REG_TMST_CONFIG                          0x54
#define IMU_TMST_TO_REGS_EN                      (0x1 << 4)
#define IMU_TMST_RES_1US                         (0x0 << 3)
#define IMU_TMST_RES_16US                        (0x1 << 3)
#define IMU_TMST_DELTA_EN                        (0x1 << 2)
#define IMU_TMST_FSYNC_EN                        (0x1 << 1)
#define IMU_TMST_EN                              (0x1 << 0)

#define REG_FIFO_CONFIG1                         0x5F
#define IMU_FIFO_RESUME_PARTIAL_RD               (0x1 << 6)
#define IMU_FIFO_WM_GT_TH                        (0x1 << 5)
#define IMU_FIFO_TMST_FSYNC_EN                   (0x1 << 3)
#define IMU_FIFO_TEMP_EN                         (0x1 << 2)
#define IMU_FIFO_GYRO_EN                         (0x1 << 1)
#define IMU_FIFO_ACCEL_EN                        (0x1 << 0)

#define REG_FIFO_CONFIG2                         0x60
#define REG_FIFO_CONFIG3                         0x61
#define IMU_FIFO_WM_UPPER_BITS_MASK              (0x0F)

#define REG_FSYNC_CONFIG                         0x62
#define IMU_FSYNC_UI_SEL_NOT_TAG                 (0x0 << 4)
#define IMU_FSYNC_UI_SEL_TAG_TEMP_LSB            (0x1 << 4)
#define IMU_FSYNC_UI_SEL_TAG_GYROX_LSB           (0x2 << 4)
#define IMU_FSYNC_UI_SEL_TAG_GYROY_LSB           (0x3 << 4)
#define IMU_FSYNC_UI_SEL_TAG_GYROZ_LSB           (0x4 << 4)
#define IMU_FSYNC_UI_SEL_TAG_ACCELX_LSB          (0x5 << 4)
#define IMU_FSYNC_UI_SEL_TAG_ACCELY_LSB          (0x6 << 4)
#define IMU_FSYNC_UI_SEL_TAG_ACCELZ_LSB          (0x7 << 4)
#define IMU_FSYNC_UI_FLAG_CLEAR_SEL_UPDATE       (0x0 << 1)
#define IMU_FSYNC_UI_FLAG_CLEAR_SEL_READ         (0x1 << 1)
#define IMU_FSYNC_POLARITY_RISING                (0x0 << 0)
#define IMU_FSYNC_POLARITY_FALLING               (0x1 << 0)

#define REG_INT_CONFIG0                          0x63
#define IMU_UI_DRDY_INT_CLEAR_ON_STATUS_READ     (0x0 << 4)
#define IMU_UI_DRDY_INT_CLEAR_ON_REG_READ        (0x2 << 4)
#define IMU_UI_DRDY_INT_CLEAR_ON_ALL_READ        (0x3 << 4)
#define IMU_FIFO_THS_INT_CLEAR_ON_STATUS_READ    (0x0 << 2)
#define IMU_FIFO_THS_INT_CLEAR_ON_FIFO_READ      (0x2 << 2)
#define IMU_FIFO_THS_INT_CLEAR_ON_ALL_READ       (0x3 << 2)
#define IMU_FIFO_FULL_INT_CLEAR_ON_STATUS_READ   (0x0 << 0)
#define IMU_FIFO_FULL_INT_CLEAR_ON_FIFO_READ     (0x2 << 0)
#define IMU_FIFO_FULL_INT_CLEAR_ON_ALL_READ      (0x3 << 0)

#define REG_INT_CONFIG1                          0x64
#define IMU_INT_TPULSE_DURATION_100US            (0x0 << 6)
#define IMU_INT_TPULSE_DURATION_8US              (0x1 << 6)
#define IMU_INT_TDEASSERT_DURATION_100US         (0x0 << 5)
#define IMU_INT_TDEASSERT_DURATION_DISABLE       (0x1 << 5)
#define IMU_INT_ASYNC_RESET                      (0x1 << 4)

#define REG_INT_SOURCE0                          0x65
#define IMU_UI_FSYNC_INT1_EN                     (0x1 << 6)
#define IMU_PLL_RDY_INT1_EN                      (0x1 << 5)
#define IMU_RESET_DONE_INT1_EN                   (0x1 << 4)
#define IMU_UI_DRDY_INT1_EN                      (0x1 << 3)
#define IMU_FIFO_THS_INT1_EN                     (0x1 << 2)
#define IMU_FIFO_FULL_INT1_EN                    (0x1 << 1)
#define IMU_UI_AGC_RDY_INT1_EN                   (0x1 << 0)

#define REG_INT_SOURCE3                          0x68
#define IMU_UI_FSYNC_INT2_EN                     (0x1 << 6)
#define IMU_PLL_RDY_INT2_EN                      (0x1 << 5)
#define IMU_RESET_DONE_INT2_EN                   (0x1 << 4)
#define IMU_UI_DRDY_INT2_EN                      (0x1 << 3)
#define IMU_FIFO_THS_INT2_EN                     (0x1 << 2)
#define IMU_FIFO_FULL_INT2_EN                    (0x1 << 1)
#define IMU_UI_AGC_RDY_INT2_EN                   (0x1 << 0)

#define REG_FIFO_LOST_PKT0                       0x6C
#define REG_FIFO_LOST_PKT1                       0x6D

#define REG_SELF_TEST_CONFIG                     0x70
#define IMU_ACCEL_ST_POWER                       (0x1 << 6)
#define IMU_AZ_ST_EN                             (0x1 << 5)
#define IMU_AY_ST_EN                             (0x1 << 4)
#define IMU_AX_ST_EN                             (0x1 << 3)
#define IMU_GZ_ST_EN                             (0x1 << 2)
#define IMU_GY_ST_EN                             (0x1 << 1)
#define IMU_GX_ST_EN                             (0x1 << 0)

#define REG_WHO_AM_I                             0x75
#define IMU_ID                                   0x42

#define REG_REG_BANK_SEL                         0x76
#define IMU_BANK_SEL_0                           (0x0 << 0)
#define IMU_BANK_SEL_1                           (0x1 << 0)
#define IMU_BANK_SEL_2                           (0x2 << 0)
#define IMU_BANK_SEL_3                           (0x3 << 0)
#define IMU_BANK_SEL_4                           (0x4 << 0)

/* --------------------------- BANK1 --------------------------- */
#define REG_GYRO_CONFIG_STATIC2                  0x0B
#define IMU_GYRO_AAF_ENABLE                      (0x0 << 1)
#define IMU_GYRO_AAF_DISABLE                     (0x1 << 1)
#define IMU_GYRO_NF_ENABLE                       (0x0 << 0)
#define IMU_GYRO_NF_DISABLE                      (0x1 << 0)

#define REG_INTF_CONFIG4                         0x7A
#define IMU_I3C_BUS_MODE_WITH_I2C                (0x0 << 6)
#define IMU_I3C_BUS_MODE_ONLY_I3C                (0x1 << 6)
#define IMU_SPI_AP_4WIRE_USE_3WIRE               (0x0 << 1)
#define IMU_SPI_AP_4WIRE_USE_4WIRE               (0x1 << 1)

#define REG_INTF_CONFIG5                         0x7B
#define IMU_PIN9_FUNCTION_INT2                   (0x0 << 1)
#define IMU_PIN9_FUNCTION_FSYNC                  (0x1 << 1)
/* --------------------------- BANK2 --------------------------- */

/* --------------------------- BANK4 --------------------------- */
#define REG_OFFSET_USER0                         0x77
#define REG_OFFSET_USER1                         0x78
#define REG_OFFSET_USER2                         0x79
#define REG_OFFSET_USER3                         0x7A
#define REG_OFFSET_USER4                         0x7B
#define REG_OFFSET_USER5                         0x7C
#define REG_OFFSET_USER6                         0x7D
#define REG_OFFSET_USER7                         0x7E
#define REG_OFFSET_USER8                         0x7F

status_t icm42605_init(void);
status_t icm42605_selftest(IMU_RAW_6DOF *imu_raw, uint8_t *result);
status_t icm42605_read(IMU_RAW_6DOF *raw, IMU_UNIT_6DOF *unit, uint32_t timeout);

status_t icm42605_gyr_offset(_3AxisUnit *gyr_off);

#endif /* DRIVER_BSP_INC_ICM42605_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
