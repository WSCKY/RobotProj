################################################################################
# subdir Makefile
# kyChu@2019-4-22
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/user_app/AttitudeEst.c \
./apps/user_app/com_task.c \
./apps/user_app/imu_calib.c \
./apps/user_app/imu_task.c \
./apps/user_app/nav_task.c

OBJS += \
$(BuildPath)/apps/user_app/AttitudeEst.o \
$(BuildPath)/apps/user_app/com_task.o \
$(BuildPath)/apps/user_app/imu_calib.o \
$(BuildPath)/apps/user_app/imu_task.o \
$(BuildPath)/apps/user_app/nav_task.o

C_DEPS += \
$(BuildPath)/apps/user_app/AttitudeEst.d \
$(BuildPath)/apps/user_app/com_task.d \
$(BuildPath)/apps/user_app/imu_calib.d \
$(BuildPath)/apps/user_app/imu_task.d \
$(BuildPath)/apps/user_app/nav_task.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/user_app/%.o: ./apps/user_app/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
