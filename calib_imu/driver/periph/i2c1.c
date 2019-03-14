/*
 * i2c1.c
 *
 *  Created on: Mar 1, 2019
 *      Author: kychu
 */

#include "i2c1.h"

/**
  * @brief  DeInitializes peripherals used by the I2C EEPROM driver.
  * @param  None
  * @retval None
  */
void sEE_LowLevel_DeInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* sEE_I2C Peripheral Disable */
  I2C_Cmd(sEE_I2C, DISABLE);

  /* sEE_I2C DeInit */
  I2C_DeInit(sEE_I2C);

  /* sEE_I2C Periph clock disable */
  RCC_APB1PeriphClockCmd(sEE_I2C_CLK, DISABLE);

  /* GPIO configuration */
  /* Configure sEE_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure sEE_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
  GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the I2C source clock and IOs used to drive the EEPROM.
  * @param  None
  * @retval None
  */
void sEE_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  /* Configure the I2C clock source. The clock is derived from the HSI (8MHz) */
  RCC_I2CCLKConfig(RCC_I2C1CLK_HSI);

  /* sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_AHBPeriphClockCmd(sEE_I2C_SCL_GPIO_CLK | sEE_I2C_SDA_GPIO_CLK, ENABLE);

  /* sEE_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(sEE_I2C_CLK, ENABLE);

  /* Connect PXx to I2C_SCL*/
  GPIO_PinAFConfig(sEE_I2C_SCL_GPIO_PORT, sEE_I2C_SCL_SOURCE, sEE_I2C_SCL_AF);

  /* Connect PXx to I2C_SDA*/
  GPIO_PinAFConfig(sEE_I2C_SDA_GPIO_PORT, sEE_I2C_SDA_SOURCE, sEE_I2C_SDA_AF);

  /* GPIO configuration */
  /* Configure sEE_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sEE_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /* Configure sEE_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = sEE_I2C_SDA_PIN;
  GPIO_Init(sEE_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}
