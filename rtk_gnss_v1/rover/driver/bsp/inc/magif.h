/*
 * magif.h
 *
 *  Created on: Dec 18, 2019
 *      Author: kychu
 */

#ifndef BSP_INC_MAGIF_H_
#define BSP_INC_MAGIF_H_

#include "SysConfig.h"

/* Definition for MAG_I2C clock resources */
#define MAG_I2C                         I2C1
#define RCC_PERIPHCLK_I2Cx              RCC_PERIPHCLK_I2C1
#define RCC_I2CxCLKSOURCE_SYSCLK        RCC_I2C1CLKSOURCE_PCLK1
#define I2Cx_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_DMA_CLK_ENABLE()           __HAL_RCC_DMA1_CLK_ENABLE()

#define I2Cx_FORCE_RESET()              __HAL_RCC_I2C1_FORCE_RESET()
#define I2Cx_RELEASE_RESET()            __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2Cx Pins */
#define I2Cx_SCL_PIN                    GPIO_PIN_8
#define I2Cx_SCL_GPIO_PORT              GPIOB
#define I2Cx_SDA_PIN                    GPIO_PIN_9
#define I2Cx_SDA_GPIO_PORT              GPIOB
#define I2Cx_SCL_SDA_AF                 GPIO_AF4_I2C1

/* Definition for I2Cx's NVIC */
#define I2Cx_EV_IRQn                    I2C1_EV_IRQn
#define I2Cx_ER_IRQn                    I2C1_ER_IRQn
#define I2Cx_EV_IRQHandler              I2C1_EV_IRQHandler
#define I2Cx_ER_IRQHandler              I2C1_ER_IRQHandler

/* Definition for I2Cx's DMA */
#define I2Cx_DMA                        DMA1
#define I2Cx_DMA_INSTANCE_TX            DMA1_Stream7
#define I2Cx_DMA_INSTANCE_RX            DMA1_Stream0
#define I2Cx_DMA_CHANNEL_TX             DMA_CHANNEL_1
#define I2Cx_DMA_CHANNEL_RX             DMA_CHANNEL_1

/* Definition for I2Cx's DMA NVIC */
#define I2Cx_DMA_TX_IRQn                DMA1_Stream7_IRQn
#define I2Cx_DMA_RX_IRQn                DMA1_Stream0_IRQn
#define I2Cx_DMA_TX_IRQHandler          DMA1_Stream7_IRQHandler
#define I2Cx_DMA_RX_IRQHandler          DMA1_Stream0_IRQHandler

status_t magif_init(void);
status_t magif_check_ready(void);
status_t magif_tx_bytes_dma(uint16_t DevAddress, uint8_t *pData, uint16_t Size);
status_t magif_read_mem_dma(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);
status_t magif_write_mem_dma(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size);

void magif_msp_init(I2C_HandleTypeDef *hi2c);
void magif_msp_deinit(I2C_HandleTypeDef *hi2c);
void magif_error_callback(I2C_HandleTypeDef *hi2c);
void magif_txcplt_callback(I2C_HandleTypeDef *hi2c);
void magif_rxcplt_callback(I2C_HandleTypeDef *hi2c);

#endif /* BSP_INC_MAGIF_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
