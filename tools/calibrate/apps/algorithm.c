/*
 * algorithm.c
 *
 *  Created on: Mar 15, 2019
 *      Author: kychu
 */

#include "algorithm.h"
#include <math.h>

float MatrixQ[ALGORITHM_SAMPLE_POINTS][3] = {0};
double MatrixA[6][6];
double MatrixB[6];
double MatrixX[6];
double MatrixM[7] = {0, 0, 0, 0, 1, 1, 1};

void reset_matrix(void)
{
	for(int i = 0; i < 6; i ++) {
		for(int j = 0; j < 6; j ++) {
			MatrixA[i][j] = 0;
		}
		MatrixB[i] = 0;
		MatrixX[i] = 0;
	}
}

#define LOOP_TIMES           6000
void EllipsoidFitting(void)
{
	int i, j, k;
	double w = 0.9, precision = 0.000000001;
	double temp, x[6] = {0, 0, 0, 0, 0, 0};
	float matrix[ALGORITHM_SAMPLE_POINTS][6];
	for(i = 0; i < ALGORITHM_SAMPLE_POINTS; i ++) {
		matrix[i][0] = MatrixQ[i][0] * MatrixQ[i][0];
		matrix[i][1] = MatrixQ[i][1] * MatrixQ[i][1];
		matrix[i][2] = MatrixQ[i][2] * MatrixQ[i][2];
		matrix[i][2] = MatrixQ[i][0];
		matrix[i][2] = MatrixQ[i][1];
		matrix[i][2] = MatrixQ[i][2];
	}

	for(i = 0; i < 6; i ++) {
		for(j = 0; j < ALGORITHM_SAMPLE_POINTS; j ++) {
			MatrixA[i][0] += matrix[j][i] * matrix[j][0];
			MatrixA[i][1] += matrix[j][i] * matrix[j][1];
			MatrixA[i][2] += matrix[j][i] * matrix[j][2];
			MatrixA[i][3] += matrix[j][i] * matrix[j][3];
			MatrixA[i][4] += matrix[j][i] * matrix[j][4];
			MatrixA[i][5] += matrix[j][i] * matrix[j][5];

			MatrixB[i] += -matrix[j][i];
		}
	}

	for(k = 0; k < LOOP_TIMES; k ++) {
		for(i = 0; i < 6; i ++) {
			temp = 0;
			for(j = 0; j < 6; j ++) {
				if(j != i)
					temp += MatrixA[i][j] * MatrixX[j];
			}
			MatrixX[i] = (1 - w) * x[i] + w * (MatrixB[i] - temp) / MatrixA[i][i];
		}
		for(i = 0; i < 6; i ++) {
			if(fabs(MatrixX[i] - x[i]) < precision) {
				if(i == 3) return;
			} else {
				break;
			}
		}
		for(i = 0; i < 6; i ++)
			x[i] = MatrixX[i];
	}

	MatrixM[0] = MatrixX[3] / 2 / MatrixX[0];
	MatrixM[1] = MatrixX[4] / 2 / MatrixX[1];
	MatrixM[2] = MatrixX[5] / 2 / MatrixX[2];
	MatrixM[3] = ((double)MatrixX[3] * MatrixX[3] / MatrixX[0] / 4 +
			      (double)MatrixX[4] * MatrixX[4] / MatrixX[1] / 4 +
				  (double)MatrixX[5] * MatrixX[5] / MatrixX[2] / 4 - 1);

	MatrixM[4] = sqrt(MatrixX[0] / MatrixM[3]);
	MatrixM[5] = sqrt(MatrixX[1] / MatrixM[3]);
	MatrixM[6] = sqrt(MatrixX[2] / MatrixM[3]);
}

void update_sample(int pos, float x, float y, float z)
{
	if(pos >= ALGORITHM_SAMPLE_POINTS) return;
	MatrixQ[pos][0] = x;
	MatrixQ[pos][1] = y;
	MatrixQ[pos][2] = z;
}

void getEllipsoidRet(float *off, float *scal)
{
	off[0] = MatrixM[0];
	off[1] = MatrixM[1];
	off[2] = MatrixM[2];
	scal[0] = MatrixM[4];
	scal[1] = MatrixM[5];
	scal[2] = MatrixM[6];
}
