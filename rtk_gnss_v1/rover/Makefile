################################################################################
# Makefile
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Project Name
ProjName = rtk_rover

# TOP path
TopPath = $(shell pwd)
# BUILD path
BuildPath = $(TopPath)/build
# Output path
OutPath = $(TopPath)/output

# Platform
PLATFORM = -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16

# Include Directories
INCS = -I$(TopPath)/startup \
       -I$(TopPath)/sysconfig \
       -I$(TopPath)/apps/inc \
       -I$(TopPath)/driver/bsp/inc \
       -I$(TopPath)/driver/cmsis/device \
       -I$(TopPath)/driver/cmsis/core \
       -I$(TopPath)/driver/stdriver/inc \
       -I$(TopPath)/driver/usbdrv/core \
       -I$(TopPath)/driver/usbdrv/cdc \
       -I$(TopPath)/middleware/fatfs \
       -I$(TopPath)/middleware/freertos/inc \
       -I$(TopPath)/middleware/algorithm/ \
       -I$(TopPath)/middleware/kylink \
       -I$(TopPath)/middleware/rb \
       -I$(TopPath)/middleware/ubx \
       -I$(TopPath)/middleware/log

LD_INCS = -I$(TopPath)/sysconfig

# get version from git
__VERSION_STR__ := $(shell git rev-parse --abbrev-ref HEAD):$(shell git rev-parse --short HEAD)

# Global definitions
DEFS = -DSTM32 -DSTM32F7 -DSTM32F732xx -DUSE_HAL_DRIVER -D__FPU_PRESENT=1 -DAPPLICATION_ADDRESS=0x08000000 -D__VERSION_STR__=$(__VERSION_STR__)

# External Library path
LIB_PATH := -L$(BuildPath)/rb \
            -L$(BuildPath)/ubx \
            -L$(BuildPath)/log \
            -L$(BuildPath)/bsp \
            -L$(BuildPath)/rtos \
            -L$(BuildPath)/fatfs \
            -L$(BuildPath)/stdrv \
            -L$(BuildPath)/kylink \
            -L$(BuildPath)/algorithm 
# External Libraries -lbsp
LIBS := -lrb -lubx -llog -lkylink -lalgorithm -lstdrv -lrtos -lfatfs

# Other configuration # -Wshadow
CFGS = -mthumb -Dprintf=iprintf -fno-exceptions -fno-builtin --short-enums -ffunction-sections -fdata-sections -fno-strict-aliasing -pipe

# linkscript configuration
# use syscalls.c instead of '-lrdimon'
# use '--specs=nano.specs' to optimize size
LD_CFGS = -lc -u _printf_float

# Initialize variables
S_SRCS :=
C_SRCS :=
OBJS :=
C_DEPS :=

ifeq ($(D), 1)
# Debug information
DBGS = -DDEBUG -g3
else
DBGS =
endif

# Set verbosity
ifeq ($(V), 1)
Q =
else
Q = @
# Do not print "Entering directory ..."
MAKEFLAGS += --no-print-directory
endif

# Commands
CC      := $(Q)arm-none-eabi-gcc
AS      := $(Q)arm-none-eabi-as
LD      := $(Q)arm-none-eabi-ld
AR      := $(Q)arm-none-eabi-ar
CPP     := $(Q)arm-none-eabi-cpp
SIZE    := $(Q)arm-none-eabi-size
STRIP   := $(Q)arm-none-eabi-strip -s
OBJCOPY := $(Q)arm-none-eabi-objcopy
OBJDUMP := $(Q)arm-none-eabi-objdump
OPENOCD ?= $(Q)openocd
RM      := $(Q)rm -rf
MKDIR   := $(Q)mkdir
ECHO    := $(Q)@echo
PRINTF  := $(Q)@printf
MAKE    := $(Q)make
JAVA    := $(Q)java
CAT     := $(Q)cat

# Export Commands
# -Operator
export Q
export CC
export AS
export LD
export AR
export SIZE
export OBJCOPY
export OBJDUMP
export MKDIR
export ECHO
# -Variables
export INCS
export DBGS
export CFGS
export DEFS
export PLATFORM
export TopPath
# export OutPath

# All Target
all: $(ProjName).elf

# All of the sources participating in the build are defined here
#-include subdir.mk
-include apps/subdir.mk
-include driver/subdir.mk
-include startup/subdir.mk
#-include middleware/subdir.mk

# user definitions
-include makefile.defs

# Add inputs and outputs from these tool invocations to the build variables

$(BuildPath):
	$(MKDIR) -p $@

$(OutPath):
	$(MKDIR) -p $@

$(OBJ_DIRS):
	$(MKDIR) -p $@

# generate elf     @$(OutPath)/objects.list
$(ProjName).elf: libraries $(OBJS) | $(OutPath)
	$(ECHO) ''
	$(ECHO) ' Building $@'
	$(CPP) -P -E $(LD_INCS) $(TopPath)/LinkerScript.ld.S > $(BuildPath)/LinkerScript.lds
	$(ECHO) $(OBJS) > $(OutPath)/objects.list
	$(CC) $(PLATFORM) $(LIB_PATH) -T$(BuildPath)/LinkerScript.lds $(LD_CFGS)  -Wl,-Map=$(OutPath)/output.map -Wl,--gc-sections -o $(OutPath)/$@ @$(OutPath)/objects.list $(LIBS) -lm
	$(MAKE) $(MAKEFLAGS) post-build

# Other Targets
clean:
	-$(RM) $(BuildPath)/ $(OutPath)/
	$(ECHO) 'Cleaned.'

libraries: 
	$(MAKE) -C middleware/rb BUILD=$(BuildPath)/rb
	$(MAKE) -C middleware/ubx BUILD=$(BuildPath)/ubx
	$(MAKE) -C middleware/log BUILD=$(BuildPath)/log
	$(MAKE) -C middleware/kylink BUILD=$(BuildPath)/kylink
	$(MAKE) -C middleware/algorithm BUILD=$(BuildPath)/algorithm
	$(MAKE) -C middleware/fatfs BUILD=$(BuildPath)/fatfs
	$(MAKE) -C middleware/freertos BUILD=$(BuildPath)/rtos
	$(MAKE) -C driver/stdriver BUILD=$(BuildPath)/stdrv
	$(MAKE) -C driver/bsp BUILD=$(BuildPath)/bsp
	$(MAKE) -C apps BUILD=$(BuildPath)/app
	$(MAKE) -C startup BUILD=$(BuildPath)/start

BINARY := $(OutPath)/$(ProjName).bin

flash: all
	@st-flash --reset write $(BINARY) 0x08000000

jflash: all
	@JLinkExe -CommanderScript f732.jlink

monitor:
	@./../../tools/monitor/Debug/monitor

post-build:
	$(ECHO) ' Generate disassembled listing for checking ...'
	$(OBJDUMP) -S $(OutPath)/$(ProjName).elf > $(OutPath)/disasm.txt
	$(ECHO) ' Generating binary and Printing size information:'
	$(OBJCOPY) -O binary $(OutPath)/$(ProjName).elf $(BINARY)
	$(SIZE) $(OutPath)/$(ProjName).elf
	$(ECHO) ' Completed.'

.PHONY: all clean flash libraries
.SECONDARY: post-build
