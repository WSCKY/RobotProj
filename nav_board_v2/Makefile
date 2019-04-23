################################################################################
# Project Makefile
# kyChu@2019-4-22
################################################################################

# Project Name
ProjName = navigation

# TOP path
TopPath = .
# BUILD path
BuildPath = $(TopPath)/Build
# Output path
OutPath = $(TopPath)/Output

# Platform
PLATFORM = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

# Include Directories
INCS = -I$(TopPath)/startup \
       -I$(TopPath)/sysconfig \
       -I$(TopPath)/apps \
       -I$(TopPath)/apps/usb_app \
       -I$(TopPath)/apps/user_app \
       -I$(TopPath)/driver/bsp/inc \
       -I$(TopPath)/driver/cmsis/device \
       -I$(TopPath)/driver/cmsis/core \
       -I$(TopPath)/driver/usb_cdc/inc \
       -I$(TopPath)/driver/usb_otg/inc \
       -I$(TopPath)/driver/stdriver/inc \
       -I$(TopPath)/middleware/maths/inc \
       -I$(TopPath)/middleware/freertos/inc \
       -I$(TopPath)/middleware/kylink

LD_INCS = -I$(TopPath)/sysconfig

# Global definitions
DEFS = -DSTM32 -DSTM32F4 -DSTM32F401CEUx -DSTM32F401xx -DUSE_STDPERIPH_DRIVER

# External Libraries
LIBS :=

# Other configuration
CFGS = -mthumb -Dprintf=iprintf -fno-exceptions
LD_CFGS = --specs=nano.specs

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
# export BuildPath
# export OutPath

# All Target
all: $(ProjName).elf | $(BuildPath)

# All of the sources participating in the build are defined here
-include subdir.mk
-include apps/subdir.mk
-include driver/subdir.mk
-include startup/subdir.mk
-include middleware/subdir.mk

# user definitions
-include makefile.defs

# Add inputs and outputs from these tool invocations to the build variables

$(BuildPath):
	$(MKDIR) -p $@

$(OutPath):
	$(MKDIR) -p $@

$(OBJ_DIRS):
	$(MKDIR) -p $@

# generate elf
$(ProjName).elf: $(OBJS) | $(OutPath)
	$(ECHO) ''
	$(ECHO) ' Building $@'
	$(CPP) -P -E $(LD_INCS) $(TopPath)/LinkerScript.ld.S > $(BuildPath)/LinkerScript.lds
	$(ECHO) $(OBJS) > $(OutPath)/objects.list
	$(CC) $(PLATFORM) -T$(BuildPath)/LinkerScript.lds $(LD_CFGS) -Wl,-Map=$(OutPath)/output.map -Wl,--gc-sections -o $(OutPath)/$(ProjName).elf @$(OutPath)/objects.list $(LIBS) -lm
	$(MAKE) $(MAKEFLAGS) post-build

# Other Targets
clean:
	-$(RM) $(BuildPath)/ $(OutPath)/
	-$(ECHO) 'Cleaned.'

BINARY := $(OutPath)/$(ProjName).bin

flash: all
	@st-flash --reset write $(BINARY) 0x08000000

post-build:
	-$(ECHO) ' Generating binary and Printing size information:'
	$(OBJCOPY) -O binary $(OutPath)/$(ProjName).elf $(OutPath)/$(ProjName).bin
	$(SIZE) $(OutPath)/$(ProjName).elf
	-$(ECHO) ' Completed.'

.PHONY: all clean dependents
.SECONDARY: post-build
