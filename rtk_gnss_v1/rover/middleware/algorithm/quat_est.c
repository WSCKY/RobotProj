/**
  ******************************************************************************
  * @file    ./quat_est.c 
  * @author  kyChu
  * @version V1.0.0
  * @date    17-April-2018
  * @brief   quaternion estimator module.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "algorithm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void fusionQ_6dot(IMU_UNIT_6DOF *unit, Quat_T *q, float prop_gain, float intg_gain, float dt)
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

	gx = unit->Gyr.X;
	gy = unit->Gyr.Y;
	gz = unit->Gyr.Z;

	ax = unit->Acc.X;
	ay = unit->Acc.Y;
	az = unit->Acc.Z;

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

void fusionQ_9dot(IMU_UNIT_9DOF *unit, Quat_T *q, float prop_gain, float intg_gain, float dt)
{
    float recipNorm;
    float qwqw, qwqx, qwqy, qwqz, qxqx, qxqy, qxqz, qyqy, qyqz, qzqz;
    float hx, hy, bx, bz;
    float halfvx, halfvy, halfvz;
    float halfex, halfey, halfez;
    float halfwx, halfwy, halfwz;
#if FREERTOS_ENABLED
  taskENTER_CRITICAL();
#endif /* FREERTOS_ENABLED */

    float twoKp = prop_gain;
    float twoKi = intg_gain;

	static float integralFBx = 0.0f;
	static float integralFBy = 0.0f;
	static float integralFBz = 0.0f;  // integral error terms scaled by Ki

    float mex, mey, mez;

    float qw, qx, qy, qz;
    float gx, gy, gz;
    float ax, ay, az;
    float mx, my, mz;

	gx = unit->Gyr.X;
	gy = unit->Gyr.Y;
	gz = unit->Gyr.Z;

	ax = unit->Acc.X;
	ay = unit->Acc.Y;
	az = unit->Acc.Z;

    gx = gx * DEG_TO_RAD;
    gy = gy * DEG_TO_RAD;
    gz = gz * DEG_TO_RAD;

    mx = unit->Mag.Y;
    my = unit->Mag.X;
    mz = -unit->Mag.Z;

	qw = q->qw;
	qx = q->qx;
	qy = q->qy;
	qz = q->qz;

    // Normalise accelerometer measurement
    if((ax != 0) || (ay != 0) || (az != 0)) {
        recipNorm =  1.0f / sqrtf(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;
    }

    // Normalise magnetometer measurement
    if((mx != 0) || (my != 0) || (mz != 0)) {
        recipNorm =  1.0f / sqrtf(mx * mx + my * my + mz * mz);
        mx *= recipNorm;
        my *= recipNorm;
        mz *= recipNorm;
    }
    // Auxiliary variables to avoid repeated arithmetic
    qwqw = qw * qw;
    qwqx = qw * qx;
    qwqy = qw * qy;
    qwqz = qw * qz;
    qxqx = qx * qx;
    qxqy = qx * qy;
    qxqz = qx * qz;
    qyqy = qy * qy;
    qyqz = qy * qz;
    qzqz = qz * qz;

    halfvx = qxqz - qwqy;
    halfvy = qwqx + qyqz;
    halfvz = qwqw - 0.5f + qzqz;

    // Reference direction of Earth's magnetic field
    hx = 2.0f * (mx * (0.5f - qyqy - qzqz) + my * (qxqy - qwqz) + mz * (qxqz + qwqy));
    hy = 2.0f * (mx * (qxqy + qwqz) + my * (0.5f - qxqx - qzqz) + mz * (qyqz - qwqx));

    bx = sqrtf(hx * hx + hy * hy);
    bz = 2.0f * (mx * (qxqz - qwqy) + my * (qyqz + qwqx) + mz * (0.5f - qxqx - qyqy));

    halfwx = bx * (0.5f - qyqy - qzqz) + bz * (qxqz - qwqy);
    halfwy = bx * (qxqy - qwqz) + bz * (qwqx + qyqz);
    halfwz = bx * (qwqy + qxqz) + bz * (0.5f - qxqx - qyqy);

    // Error is sum of cross product between estimated direction and measured direction of field vectors

    mex = (my * halfwz - mz * halfwy);
    mey = (mz * halfwx - mx * halfwz);
    mez = (mx * halfwy - my * halfwx);

    //	 mex= 0;
    //	 mey= 0;
    //	 mez= 0;

    halfex = (ay * halfvz - az * halfvy) + mex/10;
    halfey = (az * halfvx - ax * halfvz) + mey/10;
    halfez = (ax * halfvy - ay * halfvx) + mez;

    // Compute and apply integral feedback if enabled

    integralFBx += twoKi * halfex * dt;	// integral error scaled by Ki
    integralFBy += twoKi * halfey * dt;
    integralFBz += twoKi * halfez * dt;
    gx += integralFBx;	// apply integral feedback
    gy += integralFBy;
    gz += integralFBz;

    // Apply proportional feedback
    gx += twoKp * halfex;
    gy += twoKp * halfey;
    gz += twoKp * halfez;


    float delta2 = (gx * gx + gy * gy + gz * gz) * dt * dt;

    float qw_last =  qw;
    float qx_last =  qx;
    float qy_last =  qy;
    float qz_last =  qz;

    qw = qw_last * (1.0f - delta2 * 0.125f) + (-qx_last * gx - qy_last * gy - qz_last * gz) * 0.5f * dt;
    qx = qx_last * (1.0f - delta2 * 0.125f) + (qw_last * gx + qy_last * gz - qz_last * gy) * 0.5f * dt;
    qy = qy_last * (1.0f - delta2 * 0.125f) + (qw_last * gy - qx_last * gz + qz_last * gx) * 0.5f * dt;
    qz = qz_last * (1.0f - delta2 * 0.125f) + (qw_last * gz + qx_last * gy - qy_last * gx) * 0.5f * dt;

    // Normalise quaternion
    recipNorm = 1.0f / sqrtf(qw * qw + qx * qx + qy * qy + qz * qz);
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

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
