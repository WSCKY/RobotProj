################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/device/mpu9250.c

OBJS += \
$(BuildPath)/driver/device/mpu9250.o

C_DEPS += \
$(BuildPath)/driver/device/mpu9250.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/device/%.o: ./driver/device/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
