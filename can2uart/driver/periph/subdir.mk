################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/periph/CAN.c \
./driver/periph/uart1.c \
./driver/periph/uart2.c

OBJS += \
$(BuildPath)/driver/periph/CAN.o \
$(BuildPath)/driver/periph/uart1.o \
$(BuildPath)/driver/periph/uart2.o

C_DEPS += \
$(BuildPath)/driver/periph/CAN.d \
$(BuildPath)/driver/periph/uart1.d \
$(BuildPath)/driver/periph/uart2.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/periph/%.o: ./driver/periph/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
