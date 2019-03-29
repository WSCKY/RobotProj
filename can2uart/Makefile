################################################################################
# Project Makefile
# kyChu@2019-2-20
################################################################################

# Project Name
ProjName = UART2CAN

# TOP path
TopPath = $(shell pwd)
# BUILD path
BuildPath = $(TopPath)/Build
# Output path
OutPath = $(TopPath)/Output

# Include Dirs
INCS = -I$(TopPath)/apps \
       -I$(TopPath)/startup \
       -I$(TopPath)/sysconfig \
       -I$(TopPath)/middleware/kylink \
       -I$(TopPath)/driver/periph/ \
       -I$(TopPath)/driver/device/ \
       -I$(TopPath)/driver/cmsis/device \
       -I$(TopPath)/driver/cmsis/core \
       -I$(TopPath)/driver/stdriver/inc \

LD_INCS = -I$(TopPath)/sysconfig

# Global definitions
DEFS = -DSTM32 -DSTM32F0 -DSTM32F042G6Ux -DSTM32F042 -DUSE_STDPERIPH_DRIVER

# Other configuration
CFGS = -mthumb -Dprintf=iprintf -fno-exceptions
LD_CFGS = --specs=nano.specs

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
# Do not print "Entering directory ...".
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
CAT     := $(Q)cat

# Export Commands
# -Operate
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
# export BuildPath
# export OutPath

# All Target
all: $(ProjName).elf | $(BuildPath)

# All of the sources participating in the build are defined here
-include sources.mk
-include startup/subdir.mk
-include apps/subdir.mk
-include driver/device/subdir.mk
-include driver/periph/subdir.mk
-include driver/stdriver/subdir.mk
-include middleware/kylink/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# user definitions
-include makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

$(BuildPath):
	$(MKDIR) -p $@

$(OutPath):
	$(MKDIR) -p $@

$(OBJ_DIRS):
	$(MKDIR) -p $@

$(ProjName).elf: $(OBJS) | $(OutPath)
	@echo ''
	@echo ' Building $@'
	$(CPP) -P -E $(LD_INCS) $(TopPath)/LinkerScript.ld.S > $(BuildPath)/LinkerScript.lds
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -T$(BuildPath)/LinkerScript.lds $(LD_CFGS) -Wl,-Map=$(OutPath)/output.map -Wl,--gc-sections -o $(OutPath)/$(ProjName).elf @"obj_list.txt" $(LIBS) -lm
	$(MAKE) $(MAKEFLAGS) post-build

# Other Targets
clean:
	-$(RM) $(BuildPath)/ $(OutPath)/
	-@echo 'Cleaned.'

BINARY := $(OutPath)/$(ProjName).bin

flash: all
	@st-flash --reset write $(BINARY) 0x08000000

jflash: all
	@JLinkExe -CommanderScript f042.jlink

monitor:
	@./../tools/monitor/Debug/monitor

post-build:
	-@echo ' Generating binary and Printing size information:'
	$(OBJCOPY) -O binary $(OutPath)/$(ProjName).elf $(OutPath)/$(ProjName).bin
	$(SIZE) $(OutPath)/$(ProjName).elf
	-@echo ' Completed.'

.PHONY: all clean hal_lib
.SECONDARY: post-build

-include makefile.targets
