################################################################################
# startup/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
S_SRCS += \
./startup/startup_stm32.s 

C_SRCS += \
./startup/start.c \
./startup/syscalls.c \
./startup/system_stm32f7xx.c

OBJS += \
$(BuildPath)/startup/start.o \
$(BuildPath)/startup/syscalls.o \
$(BuildPath)/startup/startup_stm32.o \
$(BuildPath)/startup/system_stm32f7xx.o

C_DEPS += \
$(BuildPath)/startup/start.d \
$(BuildPath)/startup/syscalls.d \
$(BuildPath)/startup/system_stm32f7xx.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.s | $(OBJ_DIRS)
	@echo ' AS $<'
	$(AS) $(PLATFORM) $(INCS) -g -o "$@" "$<"

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
