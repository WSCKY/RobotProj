/*
 * algorithm.h
 *
 *  Created on: Mar 15, 2019
 *      Author: kychu
 */

#ifndef APPS_ALGORITHM_H_
#define APPS_ALGORITHM_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define SAMPLE_SIDES                        (6)
#define SAMPLE_POINTS_PER_SIZE              (50)
#define ALGORITHM_SAMPLE_POINTS             (SAMPLE_SIDES * SAMPLE_POINTS_PER_SIZE)

void reset_matrix(void);
void EllipsoidFitting(void);
void update_sample(int pos, float x, float y, float z);
void getEllipsoidRet(float *off, float *scal);

#endif /* APPS_ALGORITHM_H_ */
