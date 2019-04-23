/*
 * boardconfig.h
 *
 *  Created on: Apr 23, 2019
 *      Author: kychu
 */

#ifndef SYSCONFIG_BOARDCONFIG_H_
#define SYSCONFIG_BOARDCONFIG_H_

#define SYS_ARCH_STR                        "NAV IMU M4"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_HEAP_SIZE                       256
#define SYS_STACK_SIZE                      256

#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     32K
#define SYS_TEXT_ORIGIN                     0x08000000
#define SYS_TEXT_LENGTH                     480K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     96K

#define SYS_MAIN_FLASH                      FLASH_TEXT
#define SYS_MAIN_MEMORY                     SRAM

#endif /* SYSCONFIG_BOARDCONFIG_H_ */
