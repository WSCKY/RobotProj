#include "MPU9250.h"

static IMU_RAW mpu_raw_data;
#if FREERTOS_ENABLED
static QueueHandle_t mpu_queue = NULL;
#endif /* FREERTOS_ENABLED */

static uint8_t mpu9250_configured = 0;

static uint8_t mpu_tx_buffer[23] = {0};
static uint8_t mpu_rx_buffer[23] = {0};

static uint8_t MAG_ASAX = 0, MAG_ASAY = 0, MAG_ASAZ = 0;
static float AK8963_ASA[3] = {0};

static void mpu_write_reg_dma(uint8_t reg, uint8_t val);
static void mpu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r);
static void mpu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r);
static void mpu_aux_read_reg(uint8_t slaveAddr, uint8_t regAddr, uint8_t *r);
static void mpu_aux_write_reg(uint8_t slaveAddr, uint8_t regAddr, uint8_t regData);
static void mpu_aux_slaveconfig(uint8_t slaveid, uint8_t slaveAddr, uint8_t regAddr, uint8_t size);

#if FREERTOS_ENABLED
static void mpu_queue_create(void);
#endif /* FREERTOS_ENABLED */

uint8_t mag_id = 0;
uint16_t mpu_id = 0xFFFF; /* (mpu_id >> 8) = 0x71 */
/*
 * configure the MPU9250 registers.
 */
void mpu9250_init(void)
{
  mpu9250_configured = 0;
  /* initialize spi bus. */
  imu_spi_init();
  /* Communication with all registers of the device is performed using SPI at 1MHz */
  imu_spi_config_rate(IMU_SPI_SLOW_RATE);

  /* Reset the internal registers and restores the default settings. the bit will auto clear. */
  mpu_write_reg_dma(0x6B, 0x80);
  mpu_delay(10);
  /* Reset gyro & accel & temp digital signal path */
  mpu_write_reg_dma(0x68, 0x07);
  /*
    1, Disable FIFO access from serial interface. (bit6 -> 0)
    2, ENABLE I2C Master I/F module (USED FOR COMPASS). (bit5 -> 1)
    3, Disable I2C Slave module and put the serial interface in SPI mode only. (bit4 -> 1)
    4, Reset FIFO module. Reset is asynchronous. This bit auto clears after one clock cycle. (bit2 - > 1)
    5, Reset I2C Master module. Reset is asynchronous. This bit auto clears after one clock cycle. (bit1 -> 1)
    6, Reset all gyro digital signal path, accel digital signal path, and temp digital signal path. This bit also clears all the sensor registers. (bit0 -> 1)
  */
  mpu_write_reg_dma(0x6A, 0x30);
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

  /* COMPASS CONFIGURATE */
  /*
   I2C Master Control:
   1, Delays the data ready interrupt until external sensor data is loaded. (bit6 -> 1)
   2, controls the I2C Master’s transition from one slave read to the next slave read. If 1, there is a stop between reads. (bit4 -> 1)
   3, I2C_MST_CLK [3:0] -> 1101 select 400KHz.
   */
  mpu_write_reg_dma(0x24, 0x5D); mpu_delay(1);
  /*
   1, Delays shadowing of external sensor data until all data is received. (bit7 -> 1)
   */
  mpu_write_reg_dma(0x67, 0x80);
  mpu_delay(50); /* have a rest */
  mpu_aux_read_reg(0x0C, 0x00, &mag_id);
  mpu_aux_write_reg(0x0C, 0x0B, 0x01); /* When '1' is set, all registers are initialized. After reset, SRST bit turns to '0' automatically. */
  mpu_delay(50);
  mpu_aux_write_reg(0x0C, 0x0A, 0x00); /* Power-down mode */
  mpu_delay(10);
  mpu_aux_write_reg(0x0C, 0x0A, 0x1F); /* Fuse ROM access mode, Read sensitivity adjustment && 16-bit output */
  mpu_delay(10);
  mpu_aux_read_reg(0x0C, 0x10, &MAG_ASAX); mpu_delay(1);
  mpu_aux_read_reg(0x0C, 0x11, &MAG_ASAY); mpu_delay(1);
  mpu_aux_read_reg(0x0C, 0x12, &MAG_ASAZ); mpu_delay(1);
  mpu_delay(1);
  mpu_aux_write_reg(0x0C, 0x0A, 0x00); /* Power-down mode */
  mpu_delay(10);
  mpu_aux_write_reg(0x0C, 0x0A, 0x16); /* Continuous measurement mode 2 && 16-bit output */
  mpu_delay(1);
  AK8963_ASA[0] = ((MAG_ASAX - 128) * 0.5f / 128 + 1) * 0.15f;
  AK8963_ASA[1] = ((MAG_ASAY - 128) * 0.5f / 128 + 1) * 0.15f;
  AK8963_ASA[2] = ((MAG_ASAZ - 128) * 0.5f / 128 + 1) * 0.15f;
  /* add ak8963 to i2c slave 0 */
  mpu_aux_slaveconfig(1, 0x0C, 0x02, 8); /* read from ST1(Status 1) to ST2(Status 2) */
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
  unit->MagData.magX = raw->magX * AK8963_ASA[0];
  unit->MagData.magY = raw->magY * AK8963_ASA[1];
  unit->MagData.magZ = raw->magZ * AK8963_ASA[2];
  unit->TimeStamp = raw->TimeStamp;
}

void IMU_INT_Callback(void)
{
  uint32_t TS;
#if FREERTOS_ENABLED
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
#endif /* FREERTOS_ENABLED */
  if(mpu9250_configured == 1) {
    TS = _Get_Micros();
    mpu_read_reg_dma(0x3B, 22, mpu_rx_buffer);

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

    ((int8_t *)&mpu_raw_data.magX)[0] = mpu_rx_buffer[16];
    ((int8_t *)&mpu_raw_data.magX)[1] = mpu_rx_buffer[17];

    ((int8_t *)&mpu_raw_data.magY)[0] = mpu_rx_buffer[18];
    ((int8_t *)&mpu_raw_data.magY)[1] = mpu_rx_buffer[19];

    ((int8_t *)&mpu_raw_data.magZ)[0] = mpu_rx_buffer[20];
    ((int8_t *)&mpu_raw_data.magZ)[1] = mpu_rx_buffer[21];

    mpu_raw_data.TimeStamp = TS;

#if FREERTOS_ENABLED
    xQueueSendFromISR(mpu_queue, &mpu_raw_data, &xHigherPriorityTaskWoken);
#else

#endif /* FREERTOS_ENABLED */
  }
}

#if FREERTOS_ENABLED
static void mpu_queue_create(void)
{
  mpu_queue = xQueueCreate(5, sizeof(IMU_RAW));
}

QueueHandle_t* mpu_queue_get(void)
{
  return &mpu_queue;
}
#endif /* FREERTOS_ENABLED */

static void mpu_aux_slaveconfig(uint8_t slaveid, uint8_t slaveAddr, uint8_t regAddr, uint8_t size)
{
	uint8_t offset = slaveid * 3;
	mpu_write_reg_dma(0x25 + offset, slaveAddr | 0x80); /* offset from I2C_SLV0_ADDR */mpu_delay(1);
	mpu_write_reg_dma(0x26 + offset, regAddr); mpu_delay(1);
	mpu_write_reg_dma(0x27 + offset, 0x80 | size); mpu_delay(1);
	mpu_write_reg_dma(0x34, 0x09); /* this slave will only be enabled every (1 + 9) samples */mpu_delay(1);
	mpu_write_reg_dma(0x67, 0x80 | (0x01 << slaveid)); /* delayed access enabled */mpu_delay(1);
}

/*
 * write data to the specific register.
 */
static void mpu_write_reg_dma(uint8_t reg, uint8_t val)
{
	mpu_tx_buffer[0] = reg;
	mpu_tx_buffer[1] = val;
	spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2);
}

static void mpu_aux_write_reg(uint8_t slaveAddr, uint8_t regAddr, uint8_t regData)
{
	uint32_t timeout = 10;
	mpu_write_reg_dma(0x31, slaveAddr); /* I2C_SLV4_ADDR */mpu_delay(1);
	mpu_write_reg_dma(0x32, regAddr); /* I2C_SLV4_REG */mpu_delay(1);
	mpu_write_reg_dma(0x33, regData); /* I2C_SLV4_DO */mpu_delay(1);
	mpu_write_reg_dma(0x34, 0x80); /* I2C_SLV4_CTRL -> Enable data transfer */mpu_delay(1);
	mpu_tx_buffer[0] = 0x36 | 0x80;
	mpu_tx_buffer[1] = 0xFF;
	do {
		mpu_delay(1);
		spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2);
	} while(((mpu_rx_buffer[1] & 0x40) == 0) && (timeout --));
}

/*
 * read number of data from the specific register.
 */
static void mpu_read_reg_dma(uint8_t reg, uint8_t num, uint8_t *r)
{
	mpu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma(mpu_tx_buffer, r, num + 1);//ignore the first byte.
}

static void mpu_aux_read_reg(uint8_t slaveAddr, uint8_t regAddr, uint8_t *r)
{
	uint32_t timeout = 10;
	mpu_write_reg_dma(0x31, slaveAddr | 0x80); /* I2C_SLV4_ADDR */mpu_delay(1);
	mpu_write_reg_dma(0x32, regAddr); /* I2C_SLV4_REG */mpu_delay(1);
	mpu_write_reg_dma(0x34, 0x80); /* I2C_SLV4_CTRL -> Enable data transfer */mpu_delay(1);
	mpu_tx_buffer[0] = 0x36 | 0x80;
	mpu_tx_buffer[1] = 0xFF;
	do {
		mpu_delay(1);
		spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2);
	} while(((mpu_rx_buffer[1] & 0x40) == 0) && (timeout --));
	mpu_tx_buffer[0] = 0x35 | 0x80; /* I2C_SLV4_DI */
	spi_rx_tx_dma_util(mpu_tx_buffer, mpu_rx_buffer, 2);
	*r = mpu_rx_buffer[1];
}

/*
 * read number of data from specific register until the operation completed.
 */
static void mpu_read_reg_dma_util(uint8_t reg, uint8_t num, uint8_t *r)
{
	mpu_tx_buffer[0] = reg | 0x80;
	spi_rx_tx_dma_util(mpu_tx_buffer, r, num + 1);//ignore the first byte.
}
