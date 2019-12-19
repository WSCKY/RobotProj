#include "test_case.h"
#include <stdio.h>
#include "ringbuffer.h"
#include <string.h>
#include "cpu_utils.h"

void scan_i2c_dev(void);
void check_ist83xx(void);

void test_case_task(void const *argument)
{
  (void) argument;
  ky_info("test case task start.\n");
  osDelay(500);

//  scan_i2c_dev();
  check_ist83xx();

  vTaskDelete(NULL);
}

void check_ist83xx(void)
{
  int cnt = 0;
  _3AxisRaw *ist_raw_a = kmm_alloc(sizeof(_3AxisRaw));
  _3AxisRaw *ist_raw_b = kmm_alloc(sizeof(_3AxisRaw));
  ist83xx_dev_t *ist8310_a = kmm_alloc(sizeof(ist83xx_dev_t));
  ist83xx_dev_t *ist8310_b = kmm_alloc(sizeof(ist83xx_dev_t));
  if((ist8310_a == NULL) || (ist8310_b == NULL)) {
    ky_err("ist8310 memory alloc failed. EXIT!\n");
    return;
  }

  ist8310_a->dev_addr = 0x1A;
  ist8310_b->dev_addr = 0x1C;
  ist8310_a->io_init = ist8310_b->io_init = magif_init;
  ist8310_a->io_ready = ist8310_b->io_ready = magif_check_ready;
  ist8310_a->read_reg = ist8310_b->read_reg = magif_read_mem_dma;
  ist8310_a->write_reg = ist8310_b->write_reg = magif_write_mem_dma;

  ky_info("init ist8310 a.\n");
  if(ist83xx_init(ist8310_a) != status_ok) {
    ky_err("ist8310 a init failed.\n");
  }
  ky_info("init ist8310 b.\n");
  if(ist83xx_init(ist8310_b) != status_ok) {
    ky_err("ist8310 b init failed.\n");
  }

  for(;;) {
    osDelay(5);
    ist83xx_read_data(ist8310_a, ist_raw_a);
    ist83xx_read_data(ist8310_b, ist_raw_b);
    cnt ++;
    if(cnt >= 200) {
      cnt = 0;
      ky_info("%3d, %3d, %3d;  %3d, %3d, %3d\n", ist_raw_a->X, ist_raw_a->Y, ist_raw_a->Z, ist_raw_b->X, ist_raw_b->Y, ist_raw_b->Z);
    }
  }
}

void scan_i2c_dev(void)
{
  uint8_t devaddr = 0x00;
  uint8_t reg_val = 0x00;
  if(magif_init() != status_ok) {
    ky_err("mag if init failed. EXIT!\n");
    return;
  }

  osDelay(100);

  ky_info("scanning i2c device ...\n");
  for(;;) {
    reg_val = 0x00;
    magif_read_mem_dma(devaddr, 0x00, &reg_val, 1);
    osDelay(10); // wait for transfer done.
    if((devaddr & 0x1F) == 0x00)
      ky_info("\n\t");
    if(reg_val != 0x00)
      ky_info("%02x  ", devaddr);
    else
      ky_info("00  ");
    devaddr += 2;
    if(devaddr == 0x00) break;
  }
  ky_info("\n\nscan i2c device done.\n");
}
