/*
 * mpu9250.c
 *
 *  Created on: Feb 27, 2019
 *      Author: kychu
 */

#include "mpu9250.h"

static uint8_t mpu9250_configured = 0;

#if FREERTOS_ENABLED
static QueueHandle_t mpu_queue = NULL;
#else
#if !(MPU_DATA_UPDATE_HOOK_ENABLE)
static uint8_t buff_head = 0, buff_tail = 0;
static IMU_RAW mpu_buffer[MPU_DATA_DEPTH] = {0};
#endif /* MPU_DATA_UPDATE_HOOK_ENABLE */
#endif /* FREERTOS_ENABLED */

static IMU_RAW mpu_raw_data;
static uint8_t mpu_tx_buffer[15] = {0};
static uint8_t mpu_rx_buffer[15] = {0};

static void mpu_write_reg_dma(uint8_t reg, uint8_t val);
static void mpu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r);
static void mpu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r);

static void IMU_INT_Callback(void);

#if FREERTOS_ENABLED
static void mpu_queue_create(void);
#else

#endif /* FREERTOS_ENABLED */

static uint16_t mpu_id = 0xFFFF; /* (mpu_id >> 8) = 0x71 */
/*
 * configure the MPU9250 registers.
 */
uint8_t mpu9250_init(void)
{
  mpu9250_configured = 0;
  /* initialize spi bus. */
  imu_spi_init();
  /* initialize int pin */
  imu_int_init();
  imu_int_setcallback(IMU_INT_Callback);
  /* Communication with all registers of the device is performed using SPI at 1MHz */
  imu_spi_config_rate(IMU_SPI_SLOW_RATE);

  /* Reset the internal registers and restores the default settings. the bit will auto clear. */
  mpu_write_reg_dma(0x6B, 0x80);
  mpu_delay(10);
  /* Reset gyro & accel & temp digital signal path */
  mpu_write_reg_dma(0x68, 0x07);
  /*
    1, Disable FIFO access from serial interface. (bit6 -> 0)
    2, Disable I2C Master I/F module. (bit5 -> 0)
    3, Disable I2C Slave module and put the serial interface in SPI mode only. (bit4 -> 1)
    4, Reset FIFO module. Reset is asynchronous. This bit auto clears after one clock cycle. (bit2 - > 1)
    5, Reset I2C Master module. Reset is asynchronous. This bit auto clears after one clock cycle. (bit1 -> 1)
    6, Reset all gyro digital signal path, accel digital signal path, and temp digital signal path. This bit also clears all the sensor registers. (bit0 -> 1)
  */
  mpu_write_reg_dma(0x6A, 0x17);
  /* Auto selects the best available clock source - PLL if ready, else use the Internal oscillator */
  mpu_write_reg_dma(0x6B, 0x01);
  mpu_delay(10);

  /* Set sample rate to max(internal sample rate) */
  mpu_write_reg_dma(0x19, 0x00);
  /*
    1, The logic level for INT pin is active high. (bit7 -> 0)
    2, INT pin is configured as push-pull. (bit6 -> 0)
    3, INT pin indicates interrupt pulse's is width 50us. (bit5 -> 0)
    4, Interrupt status is cleared if any read operation is performed. (bit4 -> 1)
    5, The logic level for the FSYNC pin as an interrupt is active high. (bit3 -> 0)
    6, This disables the FSYNC pin from causing an interrupt. (bit2 -> 0)
  */
  mpu_write_reg_dma(0x37, 0x10);
  /* Enable Raw Sensor Data Ready interrupt to propagate to interrupt pin. */
  mpu_write_reg_dma(0x38, 0x01);
  /*
    1, when the fifo is full, additional writes will be written to the fifo, replacing the oldest data. (bit6 -> 0)
    2, disable FSYNC function.
    3, Gyroscope & Temperature DLPF. (Gyr: BW:41Hz, Delay: 5.9ms; Temp: BW:42Hz, Delay: 4.8ms)
  */
  mpu_write_reg_dma(0x1A, 0x03);
  /* Accel bypass DLPF configuration: BW:44.8Hz, Delay:4.88ms, Noise Density:300ug/rtHz */
  mpu_write_reg_dma(0x1D, 0x0B);
  /* Accel Full Scale Select: +/-8g */
  mpu_write_reg_dma(0x1C, 0x10);
  /* Gyro Full Scale Select: +/-2000dps */
  mpu_write_reg_dma(0x1B, 0x18);
  mpu_delay(1);
  /* Enable gyroscope & accelerometer(1 is disabled, 0 is on) */
  mpu_write_reg_dma(0x6C, 0x00);
  mpu_delay(1);

  /* For applications requiring faster communications, the sensor and interrupt registers may be read using SPI at 20MHz. */
  imu_spi_config_rate(IMU_SPI_FAST_RATE);

  /* This register is used to verify the identity of the device. The contents of WHO_AM_I is an 8-bit device ID. The default value of the register is 0x71. */
  mpu_read_reg_dma_util(0x75, 1, (uint8_t *)&mpu_id);

  for(uint8_t i = 0; i < 15; i ++) mpu_tx_buffer[i] = 0x00;
  mpu_delay(1);

#if FREERTOS_ENABLED
  mpu_queue_create();
#endif /* FREERTOS_ENABLED */

  mpu9250_configured = 1;
  if((mpu_id >> 8) == 0x71)
	  return 1;
  return 0;
}

void mpu_set_gyr_off(int16_t x, int16_t y, int16_t z)
{
  imu_spi_config_rate(IMU_SPI_SLOW_RATE);
  /* Configure Gyro Offset Registers */
  mpu_write_reg_dma(0x13, (uint8_t)((x) >> 8));
  mpu_write_reg_dma(0x14, (uint8_t)((x) >> 0));
  mpu_write_reg_dma(0x15, (uint8_t)((y) >> 8));
  mpu_write_reg_dma(0x16, (uint8_t)((y) >> 0));
  mpu_write_reg_dma(0x17, (uint8_t)((z) >> 8));
  mpu_write_reg_dma(0x18, (uint8_t)((z) >> 0));
  imu_spi_config_rate(IMU_SPI_FAST_RATE);
}

void mpu_raw2unit(IMU_RAW *raw, IMU_UNIT *unit)
{
  unit->AccData.accX = raw->accX * 0.002392578125f;
  unit->AccData.accY = raw->accY * 0.002392578125f;
  unit->AccData.accZ = raw->accZ * 0.002392578125f;
  unit->Temperature = 21.0f + raw->temp / 338.87f;
  unit->GyrData.gyrX = raw->gyrX * 0.06103515625f;
  unit->GyrData.gyrY = raw->gyrY * 0.06103515625f;
  unit->GyrData.gyrZ = raw->gyrZ * 0.06103515625f;
  unit->TimeStamp = raw->TimeStamp;
}

static void IMU_INT_Callback(void)
{
  uint32_t TS;
#if FREERTOS_ENABLED
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
  if(mpu9250_configured == 1) {
    TS = _Get_Micros();
    mpu_read_reg_dma(0x3B, 14, mpu_rx_buffer);

    /* organize the received data */
    ((int8_t *)&mpu_raw_data.accX)[0] = mpu_rx_buffer[2];
    ((int8_t *)&mpu_raw_data.accX)[1] = mpu_rx_buffer[1];

    ((int8_t *)&mpu_raw_data.accY)[0] = mpu_rx_buffer[4];
    ((int8_t *)&mpu_raw_data.accY)[1] = mpu_rx_buffer[3];

    ((int8_t *)&mpu_raw_data.accZ)[0] = mpu_rx_buffer[6];
    ((int8_t *)&mpu_raw_data.accZ)[1] = mpu_rx_buffer[5];

    ((int8_t *)&mpu_raw_data.temp)[0] = mpu_rx_buffer[8];
    ((int8_t *)&mpu_raw_data.temp)[1] = mpu_rx_buffer[7];

    ((int8_t *)&mpu_raw_data.gyrX)[0] = mpu_rx_buffer[10];
    ((int8_t *)&mpu_raw_data.gyrX)[1] = mpu_rx_buffer[9];

    ((int8_t *)&mpu_raw_data.gyrY)[0] = mpu_rx_buffer[12];
    ((int8_t *)&mpu_raw_data.gyrY)[1] = mpu_rx_buffer[11];

    ((int8_t *)&mpu_raw_data.gyrZ)[0] = mpu_rx_buffer[14];
    ((int8_t *)&mpu_raw_data.gyrZ)[1] = mpu_rx_buffer[13];

    mpu_raw_data.TimeStamp = TS;

#if FREERTOS_ENABLED
    xQueueSendFromISR(mpu_queue, &mpu_raw_data, &xHigherPriorityTaskWoken);
#else
#if MPU_DATA_UPDATE_HOOK_ENABLE
    mpu_update_hook(&mpu_raw_data);
#else
    mpu_push_new(&mpu_raw_data);
#endif /* MPU_DATA_UPDATE_HOOK_ENABLE */
#endif /* FREERTOS_ENABLED */
  }
}

#if FREERTOS_ENABLED
static void mpu_queue_create(void)
{
  mpu_queue = xQueueCreate(MPU_DATA_DEPTH, sizeof(IMU_RAW));
}

QueueHandle_t* mpu_queue_get(void)
{
  return &mpu_queue;
}
#else
#if MPU_DATA_UPDATE_HOOK_ENABLE
__attribute__((weak)) void mpu_update_hook(IMU_RAW *pRaw)
{

}
#else
uint8_t mpu_push_new(IMU_RAW *pRaw)
{
	if(((buff_head + 1) & MPU_BUFF_MASK) != buff_tail) {
		mpu_buffer[buff_head] = *pRaw;
		buff_head = (buff_head + 1) & MPU_BUFF_MASK;
		return 1;
	} else {
		return 0;
	}
}

uint8_t mpu_pull_new(IMU_RAW *pRaw)
{
	if(buff_tail == buff_head) return 0;
	*pRaw = mpu_buffer[buff_tail];
	buff_tail = (buff_tail + 1) & MPU_BUFF_MASK;
	return 1;
}
#endif /* MPU_DATA_UPDATE_HOOK_ENABLE */
#endif /* FREERTOS_ENABLED */

/*
 * write data to the specific register.
 */
static void mpu_write_reg_dma(uint8_t reg, uint8_t val)
{
	mpu_tx_buffer[0] = reg;
	mpu_tx_buffer[1] = val;
	spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2);
}

/*
 * read number of data from the specific register.
 */
static void mpu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r)
{
	mpu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma(mpu_tx_buffer, r, num + 1);//ignore the first byte.
}

/*
 * read number of data from specific register until the operation completed.
 */
static void mpu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r)
{
	mpu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma_util(mpu_tx_buffer, r, num + 1);//ignore the first byte.
}
