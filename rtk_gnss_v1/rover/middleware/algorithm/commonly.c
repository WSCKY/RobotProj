/**
  ******************************************************************************
  * @file    ./commonly.c
  * @author  kyChu
  * @version V1.0.0
  * @date    8-January-2020
  * @brief   commonly used functions.
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

void NormalizeVector(Vector3D *v)
{
  float recipNorm = 1.0f / sqrtf(v->X * v->X + v->Y * v->Y + v->Z * v->Z);
  v->X *= recipNorm;
  v->Y *= recipNorm;
  v->Z *= recipNorm;
}

float ScalarProduct(Vector3D *va, Vector3D *vb)
{
  return (va->X * vb->X + va->Y * vb->Y + va->Z * vb->Z);
}
