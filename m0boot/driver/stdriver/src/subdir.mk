################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/stdriver/src/stm32f0xx_crs.c \
./driver/stdriver/src/stm32f0xx_dma.c \
./driver/stdriver/src/stm32f0xx_exti.c \
./driver/stdriver/src/stm32f0xx_flash.c \
./driver/stdriver/src/stm32f0xx_gpio.c \
./driver/stdriver/src/stm32f0xx_i2c.c \
./driver/stdriver/src/stm32f0xx_misc.c \
./driver/stdriver/src/stm32f0xx_pwr.c \
./driver/stdriver/src/stm32f0xx_rcc.c \
./driver/stdriver/src/stm32f0xx_syscfg.c \
./driver/stdriver/src/stm32f0xx_tim.c \
./driver/stdriver/src/stm32f0xx_usart.c

OBJS += \
$(BuildPath)/driver/stdriver/stm32f0xx_crs.o \
$(BuildPath)/driver/stdriver/stm32f0xx_dma.o \
$(BuildPath)/driver/stdriver/stm32f0xx_exti.o \
$(BuildPath)/driver/stdriver/stm32f0xx_flash.o \
$(BuildPath)/driver/stdriver/stm32f0xx_gpio.o \
$(BuildPath)/driver/stdriver/stm32f0xx_i2c.o \
$(BuildPath)/driver/stdriver/stm32f0xx_misc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_pwr.o \
$(BuildPath)/driver/stdriver/stm32f0xx_rcc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_syscfg.o \
$(BuildPath)/driver/stdriver/stm32f0xx_tim.o \
$(BuildPath)/driver/stdriver/stm32f0xx_usart.o \

C_DEPS += \
$(BuildPath)/driver/stdriver/stm32f0xx_crs.d \
$(BuildPath)/driver/stdriver/stm32f0xx_dma.d \
$(BuildPath)/driver/stdriver/stm32f0xx_exti.d \
$(BuildPath)/driver/stdriver/stm32f0xx_flash.d \
$(BuildPath)/driver/stdriver/stm32f0xx_gpio.d \
$(BuildPath)/driver/stdriver/stm32f0xx_i2c.d \
$(BuildPath)/driver/stdriver/stm32f0xx_misc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_pwr.d \
$(BuildPath)/driver/stdriver/stm32f0xx_rcc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_syscfg.d \
$(BuildPath)/driver/stdriver/stm32f0xx_tim.d \
$(BuildPath)/driver/stdriver/stm32f0xx_usart.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/stdriver/%.o: ./driver/stdriver/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
