/*
 * ist83xx.c
 *
 *  Created on: Dec 18, 2019
 *      Author: kychu
 */

#include "ist83xx.h"

#include <math.h>

//static const char *TAG = "ISTS";

#define IST_REG_AHOAMI                           (0x00)
#  define IST_DEV_ID                             (0x10)
#define IST_REG_STATUS_1                         (0x02)
#define IST_REG_OUT_X_L                          (0x03)
#define IST_REG_OUT_X_H                          (0x04)
#define IST_REG_OUT_Y_L                          (0x05)
#define IST_REG_OUT_Y_H                          (0x06)
#define IST_REG_OUT_Z_L                          (0x07)
#define IST_REG_OUT_Z_H                          (0x08)
#define IST_REG_STATUS_2                         (0x09)
#define IST_REG_CONTROL_1                        (0x0A)
#  define OPT_MODE_MASK                          (0xF0)
#  define OPT_MODE_STANDBY                       (0x00)
#  define OPT_MODE_SINGLE_MEAS                   (0x01)
#define IST_REG_CONTROL_2                        (0x0B)
#  define DATA_RDY_MASK                          (0xF7)
#  define DATA_RDY_ENABLE                        (0x01 << 3)
#  define DATA_RDY_DISABLE                       (0x00 << 3)
#  define DRDY_POLARITY_MASK                     (0xFB)
#  define DRDY_POLARITY_LOW                      (0x00 << 2)
#  define DRDY_POLARITY_HIGH                     (0x01 << 2)
#  define SRST_ENABLE                            (0x01)
#define IST_REG_SELFTEST                         (0x0C)
#  define SELFTEST_ENABLE                        (0x40)
#  define SELFTEST_DISABLE                       (0x00)
#define IST_REG_OUT_T_L                          (0x1C)
#define IST_REG_OUT_H_L                          (0x1D)
#define IST_REG_AVGCNTL                          (0x41) // Average Control Register
#  define SENSOR_Y_AVG_MASK                      (0xC7)
#  define SENSOR_XZ_AVG_MASK                     (0xF8)
#  define SENSOR_Y_AVG_NO                        (0x00 << 3)
#  define SENSOR_Y_AVG_2                         (0x01 << 3)
#  define SENSOR_Y_AVG_4                         (0x02 << 3)
#  define SENSOR_Y_AVG_8                         (0x03 << 3)
#  define SENSOR_Y_AVG_16                        (0x04 << 3)
#  define SENSOR_XZ_AVG_NO                       (0x00 << 0)
#  define SENSOR_XZ_AVG_2                        (0x01 << 0)
#  define SENSOR_XZ_AVG_4                        (0x02 << 0)
#  define SENSOR_XZ_AVG_8                        (0x03 << 0)
#  define SENSOR_XZ_AVG_16                       (0x04 << 0)
#define IST_REG_PDCNTL                           (0x42) // Pulse Duration Control Register:0xC0
#  define PDCNTL_NORMAL                          (0xC0)
#define IST_REG_XX_CROSS_L                       (0x9C) // cross axis xx low byte
#define IST_REG_XX_CROSS_H                       (0x9D) // cross axis xx high byte
#define IST_REG_XY_CROSS_L                       (0x9E) // cross axis xy low byte
#define IST_REG_XY_CROSS_H                       (0x9F) // cross axis xy high byte
#define IST_REG_XZ_CROSS_L                       (0xA0) // cross axis xz low byte
#define IST_REG_XZ_CROSS_H                       (0xA1) // cross axis xz high byte

#define IST_REG_YX_CROSS_L                       (0xA2) // cross axis yx low byte
#define IST_REG_YX_CROSS_H                       (0xA3) // cross axis yx high byte
#define IST_REG_YY_CROSS_L                       (0xA4) // cross axis yy low byte
#define IST_REG_YY_CROSS_H                       (0xA5) // cross axis yy high byte
#define IST_REG_YZ_CROSS_L                       (0xA6) // cross axis yz low byte
#define IST_REG_YZ_CROSS_H                       (0xA7) // cross axis yz high byte

#define IST_REG_ZX_CROSS_L                       (0xA8) // cross axis zx low byte
#define IST_REG_ZX_CROSS_H                       (0xA9) // cross axis zx high byte
#define IST_REG_ZY_CROSS_L                       (0xAA) // cross axis zy low byte
#define IST_REG_ZY_CROSS_H                       (0xAB) // cross axis zy high byte
#define IST_REG_ZZ_CROSS_L                       (0xAC) // cross axis zz low byte
#define IST_REG_ZZ_CROSS_H                       (0xAD) // cross axis zz high byte

#define IST_CHECK_ASSERT(x)                      { if((x) != status_ok) return status_error; }

#if FREERTOS_ENABLED
#define ist_delay                                osDelay
#define ist_ticks                                xTaskGetTickCountFromISR
#else
#define ist_delay                                HAL_Delay
#define ist_ticks                                HAL_GetTick
#endif /* FREERTOS_ENABLE */

static status_t ist83xx_crossaxis_matrix(ist83xx_dev_t *dev);

status_t ist83xx_init(ist83xx_dev_t *dev)
{
  uint8_t vals = 0;

  if(dev == NULL) return status_error;
  if((dev->io_init == NULL) || \
     (dev->read_reg == NULL) || \
     (dev->io_ready == NULL) || \
     (dev->write_reg == NULL))
    return status_error;

  IST_CHECK_ASSERT( dev->io_init() );

  /* check device id */
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_AHOAMI, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  if(vals != IST_DEV_ID) return status_error;

  /* software reset */
  vals = SRST_ENABLE;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_CONTROL_2, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  ist_delay(200); // wait for device reset done.

  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_CONTROL_1, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  vals &= OPT_MODE_MASK;
  vals |= OPT_MODE_STANDBY; // keep in stand-by mode.
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_CONTROL_1, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  vals = PDCNTL_NORMAL;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_PDCNTL, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  vals = SENSOR_XZ_AVG_16 | SENSOR_Y_AVG_16;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_AVGCNTL, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_CONTROL_2, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  vals &= DATA_RDY_MASK & DRDY_POLARITY_MASK;
  vals |= DATA_RDY_ENABLE | DRDY_POLARITY_HIGH;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_CONTROL_2, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  IST_CHECK_ASSERT( ist83xx_crossaxis_matrix(dev) );

  /* single measurement mode */
  vals = 0x01;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_CONTROL_1, &vals, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  ist_delay(10); // wait for data ready.
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_OUT_X_L, (uint8_t *)dev->org_data, 6) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  return status_ok;
}

status_t ist83xx_read_id(ist83xx_dev_t *dev, uint8_t *id)
{
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_AHOAMI, id, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  return status_ok;
}

status_t ist83xx_check_status(ist83xx_dev_t *dev, uint8_t *result)
{
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_STATUS_1, result, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  return status_ok;
}

status_t ist83xx_read_data(ist83xx_dev_t *dev, _3AxisRaw *raw)
{
  uint8_t val;
  raw->X = dev->org_data[0] * dev->crossaxis_inv[0][0] +
           dev->org_data[1] * dev->crossaxis_inv[1][0] +
           dev->org_data[2] * dev->crossaxis_inv[2][0];
  raw->Y = dev->org_data[0] * dev->crossaxis_inv[0][1] +
           dev->org_data[1] * dev->crossaxis_inv[1][1] +
           dev->org_data[2] * dev->crossaxis_inv[2][1];
  raw->Z = dev->org_data[0] * dev->crossaxis_inv[0][2] +
           dev->org_data[1] * dev->crossaxis_inv[1][2] +
           dev->org_data[2] * dev->crossaxis_inv[2][2];
  /* single measurement mode */
  val = 0x01;
  IST_CHECK_ASSERT( dev->write_reg(dev->dev_addr, IST_REG_CONTROL_1, &val, 1) );
  // wait operation done.
  while(dev->io_ready() != status_ok) {  }; //ist_delay(1);
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_OUT_X_L, (uint8_t *)dev->org_data, 6) );
  // wait operation done.
  while(dev->io_ready() != status_ok) {  }; //ist_delay(1);
  return status_ok;
}

#define OTPsensitivity (330)
int I_InvertMatrix3by3(float *invIn, float *invOut);

static status_t ist83xx_crossaxis_matrix(ist83xx_dev_t *dev)
{
  int i, j;
  uint8_t crossxbuf[6];
  uint8_t crossybuf[6];
  uint8_t crosszbuf[6];
  float OTPcrossaxis[3][3];

  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_XX_CROSS_L, crossxbuf, 6) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_YX_CROSS_L, crossybuf, 6) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };
  IST_CHECK_ASSERT( dev->read_reg(dev->dev_addr, IST_REG_ZX_CROSS_L, crosszbuf, 6) );
  // wait operation done.
  while(dev->io_ready() != status_ok) { ist_delay(1); };

  log_write("\n\t%02x %02x %02x %02x %02x %02x", crossxbuf[0], crossxbuf[1], crossxbuf[2], crossxbuf[3], crossxbuf[4], crossxbuf[5]);
  log_write("\n\t%02x %02x %02x %02x %02x %02x", crossybuf[0], crossybuf[1], crossybuf[2], crossybuf[3], crossybuf[4], crossybuf[5]);
  log_write("\n\t%02x %02x %02x %02x %02x %02x\n", crosszbuf[0], crosszbuf[1], crosszbuf[2], crosszbuf[3], crosszbuf[4], crosszbuf[5]);

  if((crossxbuf[0] == 0xFF) && (crossxbuf[1] == 0xFF)) {
    memset(dev->crossaxis_inv, 0 ,9);
    dev->crossaxis_inv[0][0] = dev->crossaxis_inv[1][1] = dev->crossaxis_inv[2][2] = 1;
  } else {
    OTPcrossaxis[0][0] = (float)(int16_t)(((crossxbuf[1] << 8) | (crossxbuf[0]))); //328
    OTPcrossaxis[0][1] = (float)(int16_t)(((crossxbuf[3] << 8) | (crossxbuf[2]))); //9
    OTPcrossaxis[0][2] = (float)(int16_t)(((crossxbuf[5] << 8) | (crossxbuf[4]))); //19
    OTPcrossaxis[1][0] = (float)(int16_t)(((crossybuf[1] << 8) | (crossybuf[0]))); //-7
    OTPcrossaxis[1][1] = (float)(int16_t)(((crossybuf[3] << 8) | (crossybuf[2]))); //326
    OTPcrossaxis[1][2] = (float)(int16_t)(((crossybuf[5] << 8) | (crossybuf[4]))); //2
    OTPcrossaxis[2][0] = (float)(int16_t)(((crosszbuf[1] << 8) | (crosszbuf[0])));    //36
    OTPcrossaxis[2][1] = (float)(int16_t)(((crosszbuf[3] << 8) | (crosszbuf[2])));    //27
    OTPcrossaxis[2][2] = (float)(int16_t)(((crosszbuf[5] << 8) | (crosszbuf[4])));   //335
    I_InvertMatrix3by3(&OTPcrossaxis[0][0], &dev->crossaxis_inv[0][0]);
    for(i=0;i<3;i++)
      for(j=0;j<3;j++)
        dev->crossaxis_inv[i][j] = dev->crossaxis_inv[i][j] * OTPsensitivity;
  }
  return status_ok;
}

int I_InvertMatrix3by3(float *invIn, float *invOut)
{
  float largest;					// largest element used for pivoting
  float scaling;					// scaling factor in pivoting
  float recippiv;					// reciprocal of pivot element
  float ftmp;						// temporary variable used in swaps
  int32_t i, j, k, l, m;				// index counters
  int32_t iPivotRow, iPivotCol;		// row and column of pivot element
  int32_t iPivot[3]={0};
  int32_t isize=3;
  int32_t iRowInd[3] = {0};
  int32_t	iColInd[3] = {0};
  float A[3][3]={0};
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			A[i][j]= invIn[i+j*3];
		}
	//printf("%f %f %f %f",A[0][0],A[1][1],A[2][2],A[3][3]);
	// to avoid compiler warnings
	iPivotRow = iPivotCol = 0;

	// main loop i over the dimensions of the square matrix A
	for (i = 0; i < isize; i++)
	{
		// zero the largest element found for pivoting
		largest = 0.0F;
		// loop over candidate rows j
		for (j = 0; j < isize; j++)
		{
			// check if row j has been previously pivoted
			if (iPivot[j] != 1)
			{
				// loop over candidate columns k
				for (k = 0; k < isize; k++)
				{
					// check if column k has previously been pivoted
					if (iPivot[k] == 0)
					{
						// check if the pivot element is the largest found so far
						if (fabs(A[j][k]) >= largest)
						{
							// and store this location as the current best candidate for pivoting
							iPivotRow = j;
							iPivotCol = k;
							largest = (float) fabs(A[iPivotRow][iPivotCol]);
						}
					}
					else if (iPivot[k] > 1)
					{
						// zero determinant situation: exit with identity matrix
						//fmatrixAeqI(A, isize);
					 return -1;
					}
				}
			}
		}
		// increment the entry in iPivot to denote it has been selected for pivoting
		iPivot[iPivotCol]++;

		// check the pivot rows iPivotRow and iPivotCol are not the same before swapping
		if (iPivotRow != iPivotCol)
		{
			// loop over columns l
			for (l = 0; l < isize; l++)
			{
				// and swap all elements of rows iPivotRow and iPivotCol
				ftmp = A[iPivotRow][l];
				A[iPivotRow][l] = A[iPivotCol][l];
				A[iPivotCol][l] = ftmp;
			}
		}

		// record that on the i-th iteration rows iPivotRow and iPivotCol were swapped
		iRowInd[i] = iPivotRow;
		iColInd[i] = iPivotCol;

		// check for zero on-diagonal element (singular matrix) and return with identity matrix if detected
		if (A[iPivotCol][iPivotCol] == 0.0F)
		{
			// zero determinant situation: exit with identity matrix
			//fmatrixAeqI(A, isize);
			 return -1;
		}

		// calculate the reciprocal of the pivot element knowing it's non-zero
		recippiv = 1.0F / A[iPivotCol][iPivotCol];
		// by definition, the diagonal element normalizes to 1
		A[iPivotCol][iPivotCol] = 1.0F;
		// multiply all of row iPivotCol by the reciprocal of the pivot element including the diagonal element
		// the diagonal element A[iPivotCol][iPivotCol] now has value equal to the reciprocal of its previous value
		for (l = 0; l < isize; l++)
		{
			A[iPivotCol][l] *= recippiv;
		}
		// loop over all rows m of A
		for (m = 0; m < isize; m++)
		{
			if (m != iPivotCol)
			{
				// scaling factor for this row m is in column iPivotCol
				scaling = A[m][iPivotCol];
				// zero this element
				A[m][iPivotCol] = 0.0F;
				// loop over all columns l of A and perform elimination
				for (l = 0; l < isize; l++)
				{
					A[m][l] -= A[iPivotCol][l] * scaling;
				}
			}
		}
	} // end of loop i over the matrix dimensions

	// finally, loop in inverse order to apply the missing column swaps
	for (l = isize - 1; l >= 0; l--)
	{
		// set i and j to the two columns to be swapped
		i = iRowInd[l];
		j = iColInd[l];

		// check that the two columns i and j to be swapped are not the same
		if (i != j)
		{
			// loop over all rows k to swap columns i and j of A
			for (k = 0; k < isize; k++)
			{
				ftmp = A[k][i];
				A[k][i] = A[k][j];
				A[k][j] = ftmp;
			}
		}
	}
//		printf("%f %f %f %f",A[0][0],A[1][1],A[2][2],A[3][3]);
		for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{
			 invOut[i+j*3]=A[i][j];
		}
   return 0;

}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
