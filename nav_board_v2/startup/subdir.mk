################################################################################
# subdir Makefile
# kyChu@2019-4-22
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
S_SRCS += \
./startup/startup_stm32.s 

C_SRCS += \
./startup/syscalls.c \
./startup/system_stm32f4xx.c

OBJS += \
$(BuildPath)/startup/syscalls.o \
$(BuildPath)/startup/startup_stm32.o \
$(BuildPath)/startup/system_stm32f4xx.o

C_DEPS += \
$(BuildPath)/startup/syscalls.d \
$(BuildPath)/startup/system_stm32f4xx.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.s | $(OBJ_DIRS)
	@echo ' AS $<'
	$(AS) $(PLATFORM) $(INCS) -g -o "$@" "$<"

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
