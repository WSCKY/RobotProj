/*
 * bsp.c
 *
 *  Created on: Apr 29, 2019
 *      Author: kychu
 */

#include "bsp.h"

int bsp_init(void)
{
  chip_irq_initialize();
  color_led_init(); /* initialize led drive pin */ LED_R_ON();
  gpio_init();
  button_init(); /* initialize button check pin */
  osDelay(50);
  if(lsm9ds1_init() != 0) return -1;
  LED_R_OFF();
  return 0;
}
