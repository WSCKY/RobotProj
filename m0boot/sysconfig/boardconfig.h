/*
 * boardconfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#define SYS_ARCH_STR                        "ATT EST M0"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_HEAP_SIZE                       0x400
#define SYS_STACK_SIZE                      0x400

#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     6K
#define SYS_TEXT_ORIGIN                     0x08001800
#define SYS_TEXT_LENGTH                     26K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     6K

#define SYS_MAIN_FLASH                      FLASH_BOOT
#define SYS_MAIN_MEMORY                     SRAM

#define BOOT_CACHE_SIZE                     (2000)

#endif /* BOARDCONFIG_H_ */
