#include "test_case.h"
#include <stdio.h>
#include "ringbuffer.h"
#include <string.h>
#include "cpu_utils.h"
#include "ff_gen_drv.h"

void test_fatfs(void);
void test_w25qxx(void);
void scan_i2c_dev(void);
void check_ist83xx(void);
void list_file_fatfs(void);

static void print_buffer(char *tag, uint8_t *buf, uint32_t size);

void test_case_task(void const *argument)
{
  (void) argument;
  ky_info("test case task start.\n");
  osDelay(500);

//  scan_i2c_dev();
//  check_ist83xx();
//  test_w25qxx();
//  test_fatfs();
  list_file_fatfs();

  ky_info("test done.\n");

  vTaskDelete(NULL);
}

const char test_str[] = "FATFS TEST: Hello kyChu!";

void list_file_fatfs(void)
{
  FRESULT ret;
  DIR *dir;
  FILINFO *fno;
  dir = kmm_alloc(sizeof(DIR));
  fno = kmm_alloc(sizeof(FILINFO));
  if(dir == NULL || fno == NULL) {
    ky_err("no memory for test.\n");
    goto exit;
  }
  ret = f_opendir(dir, "0:/");
  if(ret != FR_OK) {
    ky_err("open dir 0:/ failed.\n");
    goto exit;
  }

  int cnt = 0;
  do {
    ret = f_readdir(dir, fno);
    if(ret != FR_OK) {
      ky_err("read dir failed.\n");
      break;
    } else if(fno->fname[0] == 0) {
      ky_info("read done.\n");
      break;
    } else {
      ky_info("%d: name: %s, size: %d, altname: %s.\n", ++cnt, fno->fname, fno->fsize, fno->altname);
    }
  } while(1);

  ret = f_closedir(dir);
  if(ret != FR_OK) {
    ky_err("close directory failed.\n");
    goto exit;
  }

exit:
  kmm_free(dir);
  kmm_free(fno);
  return;
}

void test_fatfs(void)
{
  FRESULT ret;
  uint32_t bytes_rw;
  uint8_t rtext[32];

  FIL *file;
  file = kmm_alloc(sizeof(FIL));
  if(file == NULL) {
    ky_err("no enough memory.\n");
    return;
  }

  ky_info("open file HELLO.txt\n");
  ret = f_open(file, "HELLO.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if(ret != FR_OK) {
    ky_err("failed to open/create file. %d\n", ret);
    goto exit;
  }

  ky_info("write data(%s) to file.\n", test_str);
  ret = f_write(file, test_str, sizeof(test_str), (void *)&bytes_rw);
  if(bytes_rw == 0 || ret != FR_OK) {
    ky_err("file write error. %d\n", ret);
    goto exit;
  }

  ky_info("close file.\n");
  ret = f_close(file);
  if(ret != FR_OK) {
    ky_err("failed to close file.\n");
    goto exit;
  }

  ky_info("open file again\n");
  ret = f_open(file, "HELLO.txt", FA_READ);
  if(ret != FR_OK) {
    ky_err("failed to open/read file. %d\n", ret);
    goto exit;
  }

  ky_info("read data from file.\n");
  ret = f_read(file, rtext, sizeof(rtext), (UINT*)&bytes_rw);
  if(bytes_rw == 0 || ret != FR_OK) {
    ky_err("file read error. %d\n", ret);
    goto exit;
  }

  ky_info("data in file: %s\n", rtext);

  ky_info("close file.\n");
  ret = f_close(file);
  if(ret != FR_OK) {
    ky_err("failed to close file.\n");
  }

exit:
  kmm_free(file);
  return;
}

void test_w25qxx(void)
{
  uint8_t id[3];
  uint16_t wbytes = 1000;
  uint32_t waddr = 3456;
  uint8_t *wcache, *rcache;
  if(w25qxx_init() != status_ok) {
    ky_err("failed to initialize w25qxx.\n");
    return;
  }
  if(w25qxx_read_id(id) != status_ok) {
    ky_err("read w25qxx's id failed.\n");
    return;
  }
  ky_info("w25qxx device id: 0x%x, 0x%x, 0x%x\n", id[0], id[1], id[2]);
//  switch(id) {
//  case W25Q16: ky_info("w25q16 detected.\n"); break;
//  case W25Q32: ky_info("w25q32 detected.\n"); break;
//  case W25Q64: ky_info("w25q64 detected.\n"); break;
//  case W25Q128: ky_info("w25q128 detected.\n"); break;
//  default: ky_err("unknown device.\n"); return;
//  }
//  w25qxx_erase_chip();
  if(w25qxx_write_unprotect() != status_ok) {
    ky_err("unprotect w25qxx fail.\n");
    return;
  }
  ky_info("flash read & write test.\n");
  ky_info("write %d bytes to 0x%x.\n", wbytes, waddr);
  wcache = kmm_alloc(wbytes);
  rcache = kmm_alloc(wbytes);
  if(wcache == NULL || rcache == NULL) {
    ky_err("no memory for test.\n");
    return;
  }
  memset(rcache, 0x00, wbytes);
  for(int i = 0; i < wbytes; i ++) {
    wcache[i] = i + 0x63;
  }
  print_buffer("wcache", wcache, wbytes);

  ky_info("write start ...\n");
  if(w25qxx_write_bytes(wcache, waddr, wbytes) != status_ok) {
    ky_err("write failed.\n");
    kmm_free(wcache);
    kmm_free(rcache);
    return;
  }
  osDelay(100);
  ky_info("read start ...\n");
  if(w25qxx_read_bytes(rcache, waddr, wbytes) != status_ok) {
    ky_err("read failed.\n");
    kmm_free(wcache);
    kmm_free(rcache);
    return;
  }
  print_buffer("rcache", rcache, wbytes);

//  w25qxx_erase_sector(0);
//
//  osDelay(100);
//  ky_info("read start ...\n");
//  if(w25qxx_read_bytes(rcache, waddr, wbytes) != status_ok) {
//    ky_err("read failed.\n");
//    kmm_free(wcache);
//    kmm_free(rcache);
//    return;
//  }
//  print_buffer("rcache", rcache, wbytes);

  ky_info("compare data ...\n");
  int i = 0;
  do {
    if(rcache[i] != wcache[i]) {
      break;
    }
    i ++;
  } while(i < wbytes);

  if(i < wbytes) {
    ky_err("w25qxx test failed.-%d\n", i);
  } else {
    ky_info("w25qxx test success.\n");
  }
  kmm_free(wcache);
  kmm_free(rcache);

//  for(;;) {
//	  osDelay(100);
//	  w25qxx_erase_sector(0);
//  }
}

static void print_buffer(char *tag, uint8_t *buf, uint32_t size)
{
  ky_info("\n%s: %d bytes: \n    ", tag, size);
  for(int i = 0; i < size; i ++) {
	ky_info("%02x  ", buf[i]);
	if((i & 0xF) == 0xF)
	  ky_info("\n    ");
  }
  ky_info("\n");
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
