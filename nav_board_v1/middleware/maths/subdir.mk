################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/maths/src/MathFunction.c

OBJS += \
$(BuildPath)/middleware/maths/MathFunction.o

C_DEPS += \
$(BuildPath)/middleware/maths/MathFunction.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/maths/%.o: ./middleware/maths/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
