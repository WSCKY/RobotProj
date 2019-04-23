################################################################################
# subdir Makefile
# kyChu@2019-4-22
################################################################################

-include apps/usb_app/subdir.mk
-include apps/user_app/subdir.mk

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/main.c \
./apps/stm32f4xx_it.c

OBJS += \
$(BuildPath)/apps/main.o \
$(BuildPath)/apps/stm32f4xx_it.o

C_DEPS += \
$(BuildPath)/apps/main.d \
$(BuildPath)/apps/stm32f4xx_it.d 

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
