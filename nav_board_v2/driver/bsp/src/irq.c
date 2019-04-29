/**
  ******************************************************************************
  * @file    irq.c
  * @author  kyChu
  * @version V1.0.0
  * @date    Apr 29, 2019
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "irq.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern USB_OTG_CORE_HANDLE USB_OTG_dev;
extern uint32_t USBD_OTG_ISR_Handler(USB_OTG_CORE_HANDLE * pdev);
/* Private functions ---------------------------------------------------------*/

void chip_irq_initialize(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  /* Enable External Line[9:5] Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI9_5_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable External Line[15:10] Interrupts */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = EXTI15_10_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable USART2 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART2_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA1 Stream 3 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Stream3_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA1 Stream 4 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Stream4_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA1 Stream 5 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Stream5_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

  /* Enable DMA1 Stream 6 global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA1_Stream6_IRQn_Priority;
  NVIC_Init(&NVIC_InitStructure);

//  /* Enable DMA2 Stream 3 global Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream3_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream3_IRQn_Priority;
//  NVIC_Init(&NVIC_InitStructure);

//  /* Enable DMA2 Stream 4 global Interrupt */
//  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream4_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = DMA2_Stream4_IRQn_Priority;
//  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	osSystickHandler();
}

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler(&USB_OTG_dev);
}

void EXTI0_IRQHandler(void)
{

}

void EXTI9_5_IRQHandler(void)
{
  imu_int_1_isr();
  imu_int_2_isr();
}

void EXTI15_10_IRQHandler(void)
{
  imu_int_m_isr();
  button_int_isr();
}

void DMA1_Stream3_IRQHandler(void)
{
  imu_spi_dma_rx_isr();
}

void DMA1_Stream4_IRQHandler(void)
{
  imu_spi_dma_tx_isr();
}

void DMA1_Stream5_IRQHandler(void)
{
  com_port_dma_rx_isr();
}

void DMA1_Stream6_IRQHandler(void)
{
  com_port_dma_tx_isr();
}

/******************************** END OF FILE ********************************/
