/*
 * ist83xx.h
 *
 *  Created on: Dec 18, 2019
 *      Author: kychu
 */

#ifndef BSP_INC_IST83XX_H_
#define BSP_INC_IST83XX_H_

#include "drivers.h"

typedef status_t (*i2c_hw_init)(void);
typedef status_t (*i2c_hw_ready)(void);
typedef status_t (*i2c_rw_reg)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct {
  uint8_t dev_addr;
  i2c_hw_init io_init;
  i2c_rw_reg read_reg;
  i2c_rw_reg write_reg;
  i2c_hw_ready io_ready;

  int16_t org_data[3];
  float crossaxis_inv[3][3];
} ist83xx_dev_t;

status_t ist83xx_init(ist83xx_dev_t *dev);
status_t ist83xx_read_id(ist83xx_dev_t *dev, uint8_t *id);
status_t ist83xx_read_data(ist83xx_dev_t *dev, _3AxisRaw *raw);
status_t ist83xx_check_status(ist83xx_dev_t *dev, uint8_t *result);

#endif /* BSP_INC_IST83XX_H_ */

/******************** kyChu<kyChu@qq.com> **** END OF FILE ********************/
