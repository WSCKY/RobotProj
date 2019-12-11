#include "drivers.h"

/**
  * @brief  Tx Transfer completed callback
  * @param  UartHandle: UART handle. 
  * @note   This example shows a simple way to report end of DMA Tx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if(UartHandle->Instance == UBXA_UART) {
    ubxaif_txcplt_callback(UartHandle);
  } else if(UartHandle->Instance == COM_UART) {
    comif_txcplt_callback(UartHandle);
  } else if(UartHandle->Instance == RTCM_UART) {
    rtcmif_txcplt_callback(UartHandle);
  } else if(UartHandle->Instance == EC20_UART) {
    ec20if_txcplt_callback(UartHandle);
  } else if(UartHandle->Instance == UBXB_UART) {
    ubxbif_txcplt_callback(UartHandle);
  }
  /* Set transmission flag: trasfer complete*/
  /* Transfer in transmission process is correct */
}

/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of DMA Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  if(UartHandle->Instance == UBXA_UART) {
    ubxaif_rxcplt_callback(UartHandle);
  } else if(UartHandle->Instance == COM_UART) {
    comif_rxcplt_callback(UartHandle);
  } else if(UartHandle->Instance == RTCM_UART) {
    rtcmif_rxcplt_callback(UartHandle);
  } else if(UartHandle->Instance == EC20_UART) {
    ec20if_rxcplt_callback(UartHandle);
  } else if(UartHandle->Instance == UBXB_UART) {
    ubxbif_rxcplt_callback(UartHandle);
  }
  /* Set transmission flag: trasfer complete*/
  /* Transfer in reception process is correct */
}

/**
  * @brief  UART error callbacks
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
  if(UartHandle->Instance == UBXA_UART) {
    
  } else if(UartHandle->Instance == COM_UART) {
    
  } else if(UartHandle->Instance == EC20_UART) {
    
  } else if(UartHandle->Instance == UBXB_UART) {
    
  }
}

/**
  * @brief  Tx and Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *SpiHandle)
{
  if(SpiHandle->Instance == IMU_SPI) {
    imuif_rxtxcplt_callback(SpiHandle);
  }
}
