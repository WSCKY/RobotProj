/**
  ******************************************************************************
  * @file    ./quat_est.c
  * @author  kyChu<kychu@qq.com>
  * @brief   public methods for all algorithm module.
  ******************************************************************************
  */
#ifndef __ALGORITHM_H
#define __ALGORITHM_H

#include <math.h>
#include <stdint.h>
#include "SysConfig.h"

#define LIMIT_MAX(x, y)                (((x) > (y)) ? (y) : (x))
#define LIMIT_MIN(x, y)                (((x) < (y)) ? (y) : (x))
#define LIMIT_RANGE(x, max, min)       (LIMIT_MAX(LIMIT_MIN((x), (min)), (max)))

#define ABS(x)                         (((x) > 0) ? (x) : (-(x)))

#define DEG2RAD(deg)                   ((deg) * 0.017453292519943295769236907684886f)
#define RAD2DEG(rad)                   ((rad) * 57.295779513082320876798154814105f)

typedef struct {
  float X, Y;
} Vector2D;

typedef struct {
  float X, Y, Z;
} Vector3D;

#endif /* __ALGORITHM_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/

