#include "color_led.h"

void color_led_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* LED_GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(LED_R_PORT_GPIO_CLK | LED_G_PORT_GPIO_CLK | LED_B_PORT_GPIO_CLK, ENABLE);
  /* Configure LED_GPIO_PIN in output pushpull mode */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

  GPIO_InitStructure.GPIO_Pin = LED_R_PORT_PIN;
  GPIO_Init(LED_R_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = LED_G_PORT_PIN;
  GPIO_Init(LED_G_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = LED_B_PORT_PIN;
  GPIO_Init(LED_B_PORT, &GPIO_InitStructure);

  /* Turn off all leds */
  LED_R_OFF(); LED_G_OFF(); LED_B_OFF();
}
