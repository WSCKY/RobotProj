/**
  ******************************************************************************
  * @file    ./navigation.c
  * @author  kyChu
  * @version V1.0.0
  * @date    3-January-2020
  * @brief   navigator algorithm module.
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

// direction: P1 ----> P2
double computeAzimuth(double lat1, double lon1, double lat2, double lon2) {
  if(lon2 == lon1) {
    if(lat2 > lat1)
      return 0.0f;
    else
      return 180.0f;
  }
  float alpha = 90.0f - atanf((lat2 - lat1) / (lon2 - lon1)) * RAD_TO_DEG;
  if(lon2 > lon1) return alpha;
  return (180.0f + alpha);
}

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
