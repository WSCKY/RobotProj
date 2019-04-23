#include "PwrCtrl.h"

void PWR_CTRL_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* PWR_CTRL_GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB, ENABLE);

  /* Configure PWR_CTRL_GPIO_PIN in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* default off. */
  GPIO_SetBits(GPIOA, GPIO_Pin_9);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* default off. */
  GPIO_ResetBits(GPIOB, GPIO_Pin_2);
}
