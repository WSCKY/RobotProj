/*
 * printHardFault.c
 *
 *  Created on: Mar 8, 2019
 *      Author: kychu
 */

#include "SysConfig.h"
#include <stdio.h>
#include "uart2.h"

/* Private functions ---------------------------------------------------------*/

//int __io_putchar(int ch)
//{
//	uart2_TxByte((uint8_t)ch);
//	return ch;
//}

//void print_str(char *str) {
//	while(*str != 0) {
//		uart2_TxByte(*str ++);
//	}
//}

void hard_fault_handler_c(unsigned int * hardfault_args)
{
//	char msg[80];
//    unsigned int stacked_r0;
//    unsigned int stacked_r1;
//    unsigned int stacked_r2;
//    unsigned int stacked_r3;
//    unsigned int stacked_r12;
//    unsigned int stacked_lr;
//    unsigned int stacked_pc;
//    unsigned int stacked_psr;
//
//    stacked_r0 = ((unsigned long) hardfault_args[0]);
//    stacked_r1 = ((unsigned long) hardfault_args[1]);
//    stacked_r2 = ((unsigned long) hardfault_args[2]);
//    stacked_r3 = ((unsigned long) hardfault_args[3]);
//
//    stacked_r12 = ((unsigned long) hardfault_args[4]);
//    stacked_lr = ((unsigned long) hardfault_args[5]);
//    stacked_pc = ((unsigned long) hardfault_args[6]);
//    stacked_psr = ((unsigned long) hardfault_args[7]);

//    uart2_TxBytesDMA((uint8_t *)"_HARD\n", 6);
//    sprintf(msg, "r0:0x%08x, r1:0x%08x, r2:0x%08x, r3:0x%08x.\n", stacked_r0, stacked_r1, stacked_r2, stacked_r3); print_str(msg);
//    sprintf(msg, "r12:0x%08x, lr:0x%08x, pc:0x%08x, psr:0x%08x.\n", stacked_r12, stacked_lr, stacked_pc, stacked_psr); print_str(msg);

    for(;;);
}

void HardFault_Handler(void)
{
    asm volatile(
        "movs r0, #4\t\n"
        "mov  r1, lr\t\n"
        "tst  r0, r1\t\n" /* Check EXC_RETURN[2] */
        "beq 1f\t\n"
        "mrs r0, psp\t\n"
        "ldr r1,=hard_fault_handler_c\t\n"
        "bx r1\t\n"
        "1:mrs r0,msp\t\n"
        "ldr r1,=hard_fault_handler_c\t\n"
    	"bx r1\t\n"
        : /* no output */
        : /* no input */
        : "r0" /* clobber */
    );
}
