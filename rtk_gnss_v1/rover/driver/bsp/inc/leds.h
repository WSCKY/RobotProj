#ifndef __DRIVER_BSP_LEDS_H
#define __DRIVER_BSP_LEDS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f7xx_hal.h"

typedef enum {
  LED1 = 0,
  LED_BLUE = LED1,
  LED2 = 1,
  LED_GREEN = LED2,
} Led_TypeDef;

void leds_init(void);

void led_on(Led_TypeDef led);
void led_off(Led_TypeDef led);
void led_toggle(Led_TypeDef led);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_BSP_LEDS_H */

