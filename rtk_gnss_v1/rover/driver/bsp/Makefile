#Top path
#TopPath = .
#Build path
BUILD := build
MODULE_NAME := libbsp

# Platform
#PLATFORM = -mcpu=cortex-m7 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16

#INCS = -Iinc/ \
#       -I$(TopPath)/../../driver/cmsis/device \
#       -I$(TopPath)/../../driver/cmsis/core \
#       -I$(TopPath)/../../sysconfig

# Global definitions
#DEFS = -DSTM32 -DSTM32F7 -DSTM32F732xx -DUSE_HAL_DRIVER -DAPPLICATION_ADDRESS=0x08000000

# Other configuration
#CFGS = -mthumb -Dprintf=iprintf -fno-exceptions

#ifeq ($(D), 1)
# Debug information
#DBGS = -DDEBUG -g3
#else
#DBGS =
#endif

# Set verbosity
#ifeq ($(V), 1)
#Q =
#else
#Q = @
# Do not print "Entering directory ..."
#MAKEFLAGS += --no-print-directory
#endif

#CC      := $(Q)arm-none-eabi-gcc
#AR      := $(Q)arm-none-eabi-ar
#ECHO    := $(Q)@echo

C_SRCS := $(wildcard src/*.c)
OBJS := $(addprefix $(BUILD)/,$(C_SRCS:.c=.o))
OBJ_DIRS := $(sort $(dir $(OBJS)))

all: $(MODULE_NAME).a

$(OBJ_DIRS):
	$(MKDIR) -p $@

$(BUILD)/%.o: %.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"

$(MODULE_NAME).a: $(OBJS)
	$(ECHO) ' AR $@'
	$(AR) -rcs $(BUILD)/$@ $(OBJS)

#clean:
#	@rm -rf $(BUILD) $(MODULE_NAME).a
#.PHONY: clean
.PHONY: $(MODULE_NAME).a

