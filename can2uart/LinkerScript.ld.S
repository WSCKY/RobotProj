/*
******************************************************************************
**  File        : LinkerScript.ld.S
**  Author		: kyChu
**  Date        : 2019/2/27
**  Abstract    : Linker script for STM32F042G6Ux Device from STM32F0 series
**                FLASH: 32Kbytes mapped at address 0x08000000
**                SRAM : 6Kbytes mapped at address 0x20000000
**
**  Target      : STMicroelectronics STM32F042G6Ux
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

#include "boardconfig.h"

/* Specify the memory areas */
MEMORY
{
  SRAM (xrw)      : ORIGIN = SYS_SRAM_ORIGIN, LENGTH = SYS_SRAM_LENGTH
#if defined(SYS_BOOT_ORIGIN)
  FLASH_BOOT (rx) : ORIGIN = SYS_BOOT_ORIGIN, LENGTH = SYS_BOOT_LENGTH
#endif /* defined(SYS_BOOT_ORIGIN) */
  FLASH_TEXT (rx) : ORIGIN = SYS_TEXT_ORIGIN, LENGTH = SYS_TEXT_LENGTH
}

/* Highest address of the user mode stack */
_estack = ORIGIN(SYS_MAIN_MEMORY) + LENGTH(SYS_MAIN_MEMORY); /* end of RAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = SYS_HEAP_SIZE;       /* required amount of heap  */
_Min_Stack_Size = SYS_STACK_SIZE;     /* required amount of stack */

/* Define output sections */
SECTIONS
{
  /* The startup code into FLASH memory */
  /* The program code and other data goes into FLASH */
  .text :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))// ISR table
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    . = ALIGN(4);
    *(.text*)          /* .text* sections (code) */
    . = ALIGN(4);
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
    _etext = .;        /* define a global symbols at end of code */
  } >SYS_MAIN_FLASH

  /* Constant data goes into FLASH */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)         /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
    . = ALIGN(4);
  } >SYS_MAIN_FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } >SYS_MAIN_FLASH
  .ARM : {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } >SYS_MAIN_FLASH

  .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >SYS_MAIN_FLASH
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >SYS_MAIN_FLASH
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >SYS_MAIN_FLASH

  /* used by the startup to initialize data */
  _sidata = LOADADDR(.data);
#if BOOT_ENABLED
  .ram_isr (NOLOAD) :
  {
    . = ALIGN(4);
    . = . + SYS_VECTOR_SIZE;
  } >SYS_MAIN_MEMORY
#endif /* BOOT_ENABLED */

  .usc_uart_cache (NOLOAD) :
  {
    . = ALIGN(4);
    UART_RX_CACHE = .;
    . = . + UART_RX_CACHE_SIZE;
  } >SYS_MAIN_MEMORY

  /* Initialized data sections goes into RAM, load LMA copy after code */
  .data : 
  {
    . = ALIGN(4);
    _sdata = .;        /* create a global symbol at data start */
    *(.data)           /* .data sections */
    *(.data*)          /* .data* sections */

    . = ALIGN(4);
    _edata = .;        /* define a global symbol at data end */
  } >SYS_MAIN_MEMORY AT> SYS_MAIN_FLASH

  /* Uninitialized data section */
  . = ALIGN(4);
  .bss (NOLOAD) :
  {
    /* This is used by the startup in order to initialize the .bss secion */
    _sbss = .;         /* define a global symbol at bss start */
    __bss_start__ = _sbss;
    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;         /* define a global symbol at bss end */
    __bss_end__ = _ebss;
  } >SYS_MAIN_MEMORY

  /* User_heap_stack section, used to check that there is enough RAM left */
  ._user_heap_stack (NOLOAD) :
  {
    . = ALIGN(8);
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;
    . = ALIGN(8);
  } >SYS_MAIN_MEMORY

  /* Remove information from the standard libraries */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
