#include "test_case.h"
#include <stdio.h>
#include "ringbuffer.h"
#include <string.h>
#include "cpu_utils.h"

void scan_i2c_dev(void);

void test_case_task(void const *argument)
{
  (void) argument;
  ky_info("test case task start.\n");
  osDelay(500);

  scan_i2c_dev();

  vTaskDelete(NULL);
}

void scan_i2c_dev(void)
{
  uint8_t devaddr = 0x00;
  uint8_t reg_val = 0x00;
  if(magif_init() != status_ok) {
    ky_info("mag if init failed. EXIT!\n");
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
