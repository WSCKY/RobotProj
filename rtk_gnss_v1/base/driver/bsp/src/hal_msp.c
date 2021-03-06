/**
  ******************************************************************************
  * @file    driver/hal_msp.c
  * @author  kyChu
  * @brief   HAL MSP module.    
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "drivers.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief UART MSP Initialization 
  *        This function configures the hardware resources used in this example: 
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration  
  *           - DMA configuration for transmission request by peripheral 
  *           - NVIC configuration for DMA interrupt request enable
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UBXA_UART) {
    ubxaif_msp_init(huart);
  } else if(huart->Instance == COM_UART) {
    comif_msp_init(huart);
  } else if(huart->Instance == RTCM_UART) {
    rtcmif_msp_init(huart);
  } else if(huart->Instance == EC20_UART) {
    ec20if_msp_init(huart);
  } else if(huart->Instance == UBXB_UART) {
    ubxbif_msp_init(huart);
  }
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if(huart->Instance == UBXA_UART) {
    ubxaif_msp_deinit(huart);
  } else if(huart->Instance == COM_UART) {
    comif_msp_deinit(huart);
  } else if(huart->Instance == RTCM_UART) {
    rtcmif_msp_deinit(huart);
  } else if(huart->Instance == EC20_UART) {
    ec20if_msp_deinit(huart);
  } else if(huart->Instance == UBXB_UART) {
    ubxbif_msp_deinit(huart);
  }
}

/**
  * @brief SPI MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == IMU_SPI) {
    imuif_msp_init(hspi);
  }
}

/**
  * @brief SPI MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param hspi: SPI handle pointer
  * @retval None
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == IMU_SPI) {
    imuif_msp_deinit(hspi);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
