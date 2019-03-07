################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/periph/uart2.c \
./driver/periph/i2c1.c \
./driver/periph/spi1.c \
./driver/periph/intio.c

OBJS += \
$(BuildPath)/driver/periph/uart2.o \
$(BuildPath)/driver/periph/spi1.o \
$(BuildPath)/driver/periph/i2c1.o \
$(BuildPath)/driver/periph/intio.o

C_DEPS += \
$(BuildPath)/driver/periph/uart2.d \
$(BuildPath)/driver/periph/spi1.d \
$(BuildPath)/driver/periph/i2c1.d \
$(BuildPath)/driver/periph/intio.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/periph/%.o: ./driver/periph/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
