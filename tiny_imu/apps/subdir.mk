################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/main_task.c \
./apps/imu_task.c \
./apps/com_task.c \
./apps/imu_calib.c \
./apps/AttitudeEst.c

OBJS += \
$(BuildPath)/apps/main_task.o \
$(BuildPath)/apps/imu_task.o \
$(BuildPath)/apps/com_task.o \
$(BuildPath)/apps/imu_calib.o \
$(BuildPath)/apps/AttitudeEst.o

C_DEPS += \
$(BuildPath)/apps/main_task.d \
$(BuildPath)/apps/imu_task.d \
$(BuildPath)/apps/com_task.d \
$(BuildPath)/apps/imu_calib.d \
$(BuildPath)/apps/AttitudeEst.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
