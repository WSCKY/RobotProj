/*
 * AttEst.h
 *
 *  Created on: Feb 28, 2019
 *      Author: kychu
 */

#ifndef __ATTEST_H
#define __ATTEST_H

#include "mpu9250.h"

#include "maths.h"

Quat_T get_est_q(void);
IMU_UNIT get_imu_unit(void);

#endif /* __ATTEST_H */
