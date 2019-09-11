################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/device/mpu9250.c \
./driver/device/eeprom.c

OBJS += \
$(BuildPath)/driver/device/mpu9250.o \
$(BuildPath)/driver/device/eeprom.o

C_DEPS += \
$(BuildPath)/driver/device/mpu9250.d \
$(BuildPath)/driver/device/eeprom.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/device/%.o: ./driver/device/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
