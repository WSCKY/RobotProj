/**
  ******************************************************************************
  * @file    ./src/Maths.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   user math module.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "maths.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*
 * apply deadband function.
 */
//float apply_deadband(float value, float deadband)
//{
//	if(fabs(value) <= deadband) {value = 0;}
//
//	if(value > deadband) {value -= deadband;}
//	if(value < -deadband) {value += deadband;}
//
//	return value;
//}
//
///*
// * step change function.
// */
//void step_change(float *in, float target, float step, float deadBand)
//{
//	if(fabsf(*in - target) <= deadBand) {*in = target; return;}
//
//	if(*in > target) {*in -= fabsf(step); return;}
//	if(*in < target) {*in += fabsf(step); return;}
//}

/*
 * CRC8 table.
 */
//const uint8_t CRC8_TAB[256] =
//{
//	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,   // 0-F 16 perline
//	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,   // 10-1F
//	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,   // 20-2F
//	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,   // 30-3F
//	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,   // 40-4f
//	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,   // 50-5f
//	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,   // 60-6f
//	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,   // 70-7f
//	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,   // 80-8f
//	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,   // 90-9f
//	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,   // A0-Af
//	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,   // B0-Bf
//	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,   // C0-Cf
//	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,   // D0-Df
//	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,   // E0-Ef
//	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,   // F0-Ff
//};

/*
 *caculate crc value by lookup table
 */
//uint8_t ComputeCRC8(uint8_t *pchMessage, uint32_t dwLength, uint8_t ucCRC8)
//{
//	uint8_t ucIndex;
//
//	while(dwLength--)
//	{
//		ucIndex = ucCRC8^(*pchMessage++);
//		ucCRC8  = CRC8_TAB[ucIndex];
//	}
//
//	return (ucCRC8);
//}

void fusionQ_6dot(IMU_UNIT *unit, Quat_T *q, float prop_gain, float intg_gain, float dt)
{
	float recipNorm;
	float halfvx, halfvy, halfvz;
	float halfex, halfey, halfez;
#if FREERTOS_ENABLED
  taskENTER_CRITICAL();
#endif /* FREERTOS_ENABLED */

	float twoKp = prop_gain;    // 2 * proportional gain (Kp)
	float twoKi = intg_gain;    // 2 * integral gain (Ki)

	static float integralFBx = 0.0f;
	static float integralFBy = 0.0f;
	static float integralFBz = 0.0f;  // integral error terms scaled by Ki

	float qw, qx, qy, qz;
	float gx, gy, gz;
	float ax, ay, az;

	qw = q->qw;
	qx = q->qx;
	qy = q->qy;
	qz = q->qz;

	gx = unit->GyrData.gyrX;
	gy = unit->GyrData.gyrY;
	gz = unit->GyrData.gyrZ;

	ax = unit->AccData.accX;
	ay = unit->AccData.accY;
	az = unit->AccData.accZ;

	gx *= DEG_TO_RAD;
	gy *= DEG_TO_RAD;
	gz *= DEG_TO_RAD;

	// Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
	{
		// Normalise accelerometer measurement
		recipNorm = 1.0f/sqrtf(ax * ax + ay * ay + az * az);
		ax *= recipNorm;
		ay *= recipNorm;
		az *= recipNorm;

		// Estimated direction of gravity and vector perpendicular to magnetic flux
		halfvx = qx * qz - qw * qy;
		halfvy = qw * qx + qy * qz;
		halfvz = qw * qw - 0.5f + qz * qz;

		// Error is sum of cross product between estimated and measured direction of gravity
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);

		// Compute and apply integral feedback if enabled

		integralFBx += twoKi * halfex * dt;  // integral error scaled by Ki
		integralFBy += twoKi * halfey * dt;
		integralFBz += twoKi * halfez * dt;
		gx += integralFBx;  // apply integral feedback
		gy += integralFBy;
		gz += integralFBz;

		// Apply proportional feedback
		gx += twoKp * halfex;
		gy += twoKp * halfey;
		gz += twoKp * halfez;
	}

	float delta2 = (gx*gx + gy*gy + gz*gz)*dt*dt;

	float qw_last = qw;
	float qx_last = qx;
	float qy_last = qy;
	float qz_last = qz;

	qw = qw_last*(1.0f-delta2*0.125f) + (-qx_last*gx - qy_last*gy - qz_last*gz)*0.5f * dt;
	qx = qx_last*(1.0f-delta2*0.125f) + (qw_last*gx + qy_last*gz - qz_last*gy)*0.5f * dt;
	qy = qy_last*(1.0f-delta2*0.125f) + (qw_last*gy - qx_last*gz + qz_last*gx)*0.5f * dt;
	qz = qz_last*(1.0f-delta2*0.125f) + (qw_last*gz + qx_last*gy - qy_last*gx)*0.5f * dt;

	// Normalise quaternion
	recipNorm = 1.0f/sqrtf(qw * qw + qx * qx + qy * qy + qz * qz);
	qw *= recipNorm;
	qx *= recipNorm;
	qy *= recipNorm;
	qz *= recipNorm;

	q->qw = qw;
	q->qx = qx;
	q->qy = qy;
	q->qz = qz;

#if FREERTOS_ENABLED
  taskEXIT_CRITICAL();
#endif /* FREERTOS_ENABLED */
}

void Quat2Euler(Quat_T* q, Euler_T* eur)
{
	float qw = q->qw;
	float qx = q->qx;
	float qy = q->qy;
	float qz = q->qz;

	eur->roll    = atan2f(2 * (qw * qx + qy * qz) , 1 - 2 * (qx * qx + qy * qy))*RAD_TO_DEG;  //+-90      
	eur->pitch   = asinf(2 * (qw * qy - qz * qx))*RAD_TO_DEG;                                 //+-180   
	eur->yaw     = atan2f(2 * (qw * qz + qx * qy) , 1 - 2 * (qy * qy + qz * qz))*RAD_TO_DEG;  //+-180   
}

/**
  * Return Version String.
  */
//uint8_t* GetVersionString(void) { return (uint8_t *)MATH_FUNCTION_VERSION_STR; }

/******************************** END OF FILE *********************************/
