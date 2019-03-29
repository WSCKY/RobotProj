################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
./startup/startup_stm32.s 

C_SRCS += \
./startup/start.c \
./startup/syscalls.c \
./startup/TimerCounter.c \
./startup/stm32f0xx_it.c \
./startup/system_stm32f0xx.c

OBJS += \
$(BuildPath)/startup/startup_stm32.o \
$(BuildPath)/startup/start.o\
$(BuildPath)/startup/syscalls.o \
$(BuildPath)/startup/TimerCounter.o \
$(BuildPath)/startup/stm32f0xx_it.o \
$(BuildPath)/startup/system_stm32f0xx.o

C_DEPS += \
$(BuildPath)/startup/start.d\
$(BuildPath)/startup/syscalls.d \
$(BuildPath)/startup/TimerCounter.d \
$(BuildPath)/startup/stm32f0xx_it.d \
$(BuildPath)/startup/system_stm32f0xx.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.s | $(OBJ_DIRS)
	@echo ' AS $<'
	$(AS) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(INCS) -g -o "$@" "$<"

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/startup/%.o: ./startup/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
