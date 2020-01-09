################################################################################
# algorithm/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/algorithm/commonly.c \
./middleware/algorithm/quat_est.c \
./middleware/algorithm/navigation.c

OBJS += \
$(BuildPath)/middleware/algorithm/commonly.o \
$(BuildPath)/middleware/algorithm/quat_est.o \
$(BuildPath)/middleware/algorithm/navigation.o 

C_DEPS += \
$(BuildPath)/middleware/algorithm/commonly.d \
$(BuildPath)/middleware/algorithm/quat_est.d \
$(BuildPath)/middleware/algorithm/navigation.o

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/algorithm/%.o: ./middleware/algorithm/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
