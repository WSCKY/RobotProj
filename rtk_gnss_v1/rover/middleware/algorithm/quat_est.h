/**
  ******************************************************************************
  * @file    ./quat_est.c
  * @author  kyChu<kychu@qq.com>
  * @brief   Header file for quat_est.c file.
  ******************************************************************************
  */
#ifndef __QUAT_EST_H
#define __QUAT_EST_H

#include "algorithm.h"

#define DEG_TO_RAD 0.017453292519943295769236907684886f
#define RAD_TO_DEG 57.295779513082320876798154814105f

void fusionQ_6dot(IMU_UNIT_6DOF *unit, Quat_T *q, float prop_gain, float intg_gain, float dt);
void fusionQ_9dot(IMU_UNIT_9DOF *unit, Quat_T *q, float prop_gain, float intg_gain, float dt);
void Quat2Euler(Quat_T* q, Euler_T* eur);

#endif /* __QUAT_EST_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/

