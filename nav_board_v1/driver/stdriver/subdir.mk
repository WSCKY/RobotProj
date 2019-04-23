################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/stdriver/src/misc.c \
./driver/stdriver/src/stm32f4xx_dma.c \
./driver/stdriver/src/stm32f4xx_exti.c \
./driver/stdriver/src/stm32f4xx_gpio.c \
./driver/stdriver/src/stm32f4xx_i2c.c \
./driver/stdriver/src/stm32f4xx_rcc.c \
./driver/stdriver/src/stm32f4xx_spi.c \
./driver/stdriver/src/stm32f4xx_syscfg.c \
./driver/stdriver/src/stm32f4xx_tim.c \
./driver/stdriver/src/stm32f4xx_usart.c

OBJS += \
$(BuildPath)/driver/stdriver/misc.o \
$(BuildPath)/driver/stdriver/stm32f4xx_dma.o \
$(BuildPath)/driver/stdriver/stm32f4xx_exti.o \
$(BuildPath)/driver/stdriver/stm32f4xx_gpio.o \
$(BuildPath)/driver/stdriver/stm32f4xx_i2c.o \
$(BuildPath)/driver/stdriver/stm32f4xx_rcc.o \
$(BuildPath)/driver/stdriver/stm32f4xx_spi.o \
$(BuildPath)/driver/stdriver/stm32f4xx_syscfg.o \
$(BuildPath)/driver/stdriver/stm32f4xx_tim.o \
$(BuildPath)/driver/stdriver/stm32f4xx_usart.o

C_DEPS += \
$(BuildPath)/driver/stdriver/misc.d \
$(BuildPath)/driver/stdriver/stm32f4xx_dma.d \
$(BuildPath)/driver/stdriver/stm32f4xx_exti.d \
$(BuildPath)/driver/stdriver/stm32f4xx_gpio.d \
$(BuildPath)/driver/stdriver/stm32f4xx_i2c.d \
$(BuildPath)/driver/stdriver/stm32f4xx_rcc.d \
$(BuildPath)/driver/stdriver/stm32f4xx_spi.d \
$(BuildPath)/driver/stdriver/stm32f4xx_syscfg.d \
$(BuildPath)/driver/stdriver/stm32f4xx_tim.d \
$(BuildPath)/driver/stdriver/stm32f4xx_usart.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/stdriver/%.o: ./driver/stdriver/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
