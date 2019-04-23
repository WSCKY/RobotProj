#include "Button.h"

#if FREERTOS_ENABLED
static osSemaphoreId osSemaphore;
#endif /* FREERTOS_ENABLED */

/**
  * Initialize Button GPIO Pin in EXTI mode.
  */
#if FREERTOS_ENABLED
  osSemaphoreId
#else
  void
#endif /* FREERTOS_ENABLED */
ButtonInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	/* BUTTON_GPIO Periph clock enable */
  RCC_AHB1PeriphClockCmd(USER_BUTTON_GPIO_CLK, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure BUTTON_GPIO_PIN in Input mode */
  GPIO_InitStructure.GPIO_Pin = USER_BUTTON_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(USER_BUTTON_GPIO_PORT, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(USER_BUTTON_EXTI_PORT_SOURCE, USER_BUTTON_EXTI_PIN_SOURCE);

  /* Configure Button EXTI line */
  EXTI_InitStructure.EXTI_Line = USER_BUTTON_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set Button EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = USER_BUTTON_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = INT_PRIORITY_BUTTON;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);

#if FREERTOS_ENABLED
  /* Define used semaphore */
  osSemaphoreDef(BTN);
  /* Create the semaphore used by the two threads. */
  osSemaphore = osSemaphoreCreate(osSemaphore(BTN) , 1);
  return osSemaphore;
#endif /* FREERTOS_ENABLED */
}

#if FREERTOS_ENABLED
osSemaphoreId GetButtonSemaphore(void) { return osSemaphore; }
#endif /* FREERTOS_ENABLED */

void button_int_isr(void)
{
  if(EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) != RESET) {
#if FREERTOS_ENABLED
    osSemaphoreRelease(osSemaphore);
#else
    ButtonPressedCallback();
#endif /* FREERTOS_ENABLED */
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}
