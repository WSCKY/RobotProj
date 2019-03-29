/*
 * boardconfig.h
 *
 *  Created on: Feb 19, 2019
 *      Author: kychu
 */

#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#define SYS_ARCH_STR                        "UART2CAN M0"

#define SYS_XCLK_FREQUENCY                  (8000000)

#define SYS_HEAP_SIZE                       256
#define SYS_STACK_SIZE                      256

#define BOOT_ENABLED                        0

#if BOOT_ENABLED
#define SYS_BOOT_ORIGIN                     0x08000000
#define SYS_BOOT_LENGTH                     6K
#endif /* BOOT_ENABLED */
#define SYS_TEXT_ORIGIN                     0x08000000
#define SYS_TEXT_LENGTH                     32K
#define SYS_SRAM_ORIGIN                     0x20000000
#define SYS_SRAM_LENGTH                     6K

#define SYS_MAIN_FLASH                      FLASH_TEXT
#define SYS_MAIN_MEMORY                     SRAM

#if BOOT_ENABLED
/* reserve 48 word starting in RAM @ 0x20000000 */
#define SYS_VECTOR_SIZE                     192
#endif /* BOOT_ENABLED */

#define UART_RX_CACHE_SIZE                  88

#endif /* BOARDCONFIG_H_ */
