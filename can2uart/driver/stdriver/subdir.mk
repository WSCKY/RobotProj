################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/stdriver/src/stm32f0xx_adc.c \
./driver/stdriver/src/stm32f0xx_can.c \
./driver/stdriver/src/stm32f0xx_cec.c \
./driver/stdriver/src/stm32f0xx_comp.c \
./driver/stdriver/src/stm32f0xx_crc.c \
./driver/stdriver/src/stm32f0xx_crs.c \
./driver/stdriver/src/stm32f0xx_dac.c \
./driver/stdriver/src/stm32f0xx_dbgmcu.c \
./driver/stdriver/src/stm32f0xx_dma.c \
./driver/stdriver/src/stm32f0xx_exti.c \
./driver/stdriver/src/stm32f0xx_flash.c \
./driver/stdriver/src/stm32f0xx_gpio.c \
./driver/stdriver/src/stm32f0xx_i2c.c \
./driver/stdriver/src/stm32f0xx_iwdg.c \
./driver/stdriver/src/stm32f0xx_misc.c \
./driver/stdriver/src/stm32f0xx_pwr.c \
./driver/stdriver/src/stm32f0xx_rcc.c \
./driver/stdriver/src/stm32f0xx_rtc.c \
./driver/stdriver/src/stm32f0xx_spi.c \
./driver/stdriver/src/stm32f0xx_syscfg.c \
./driver/stdriver/src/stm32f0xx_tim.c \
./driver/stdriver/src/stm32f0xx_usart.c \
./driver/stdriver/src/stm32f0xx_wwdg.c 

OBJS += \
$(BuildPath)/driver/stdriver/stm32f0xx_adc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_can.o \
$(BuildPath)/driver/stdriver/stm32f0xx_cec.o \
$(BuildPath)/driver/stdriver/stm32f0xx_comp.o \
$(BuildPath)/driver/stdriver/stm32f0xx_crc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_crs.o \
$(BuildPath)/driver/stdriver/stm32f0xx_dac.o \
$(BuildPath)/driver/stdriver/stm32f0xx_dbgmcu.o \
$(BuildPath)/driver/stdriver/stm32f0xx_dma.o \
$(BuildPath)/driver/stdriver/stm32f0xx_exti.o \
$(BuildPath)/driver/stdriver/stm32f0xx_flash.o \
$(BuildPath)/driver/stdriver/stm32f0xx_gpio.o \
$(BuildPath)/driver/stdriver/stm32f0xx_i2c.o \
$(BuildPath)/driver/stdriver/stm32f0xx_iwdg.o \
$(BuildPath)/driver/stdriver/stm32f0xx_misc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_pwr.o \
$(BuildPath)/driver/stdriver/stm32f0xx_rcc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_rtc.o \
$(BuildPath)/driver/stdriver/stm32f0xx_spi.o \
$(BuildPath)/driver/stdriver/stm32f0xx_syscfg.o \
$(BuildPath)/driver/stdriver/stm32f0xx_tim.o \
$(BuildPath)/driver/stdriver/stm32f0xx_usart.o \
$(BuildPath)/driver/stdriver/stm32f0xx_wwdg.o 

C_DEPS += \
$(BuildPath)/driver/stdriver/stm32f0xx_adc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_can.d \
$(BuildPath)/driver/stdriver/stm32f0xx_cec.d \
$(BuildPath)/driver/stdriver/stm32f0xx_comp.d \
$(BuildPath)/driver/stdriver/stm32f0xx_crc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_crs.d \
$(BuildPath)/driver/stdriver/stm32f0xx_dac.d \
$(BuildPath)/driver/stdriver/stm32f0xx_dbgmcu.d \
$(BuildPath)/driver/stdriver/stm32f0xx_dma.d \
$(BuildPath)/driver/stdriver/stm32f0xx_exti.d \
$(BuildPath)/driver/stdriver/stm32f0xx_flash.d \
$(BuildPath)/driver/stdriver/stm32f0xx_gpio.d \
$(BuildPath)/driver/stdriver/stm32f0xx_i2c.d \
$(BuildPath)/driver/stdriver/stm32f0xx_iwdg.d \
$(BuildPath)/driver/stdriver/stm32f0xx_misc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_pwr.d \
$(BuildPath)/driver/stdriver/stm32f0xx_rcc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_rtc.d \
$(BuildPath)/driver/stdriver/stm32f0xx_spi.d \
$(BuildPath)/driver/stdriver/stm32f0xx_syscfg.d \
$(BuildPath)/driver/stdriver/stm32f0xx_tim.d \
$(BuildPath)/driver/stdriver/stm32f0xx_usart.d \
$(BuildPath)/driver/stdriver/stm32f0xx_wwdg.d 

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/stdriver/%.o: ./driver/stdriver/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) -Os -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
