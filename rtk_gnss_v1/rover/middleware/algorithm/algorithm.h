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

#endif /* __ALGORITHM_H */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/

