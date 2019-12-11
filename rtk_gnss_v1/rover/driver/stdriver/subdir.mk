################################################################################
# stdriver/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += $(addprefix ./driver/stdriver/src/, \
stm32f7xx_hal.c \
stm32f7xx_hal_gpio.c \
stm32f7xx_hal_rcc.c \
stm32f7xx_hal_pwr.c \
stm32f7xx_hal_pwr_ex.c \
stm32f7xx_hal_cortex.c \
stm32f7xx_hal_dma.c \
stm32f7xx_hal_i2c.c \
stm32f7xx_hal_i2c_ex.c \
stm32f7xx_hal_rcc_ex.c \
stm32f7xx_hal_tim.c \
stm32f7xx_hal_tim_ex.c \
stm32f7xx_hal_uart.c \
stm32f7xx_hal_spi.c \
)

OBJS += \
$(BuildPath)/driver/stdriver/stm32f7xx_hal.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_gpio.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_rcc.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_pwr.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_pwr_ex.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_cortex.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_dma.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_i2c.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_i2c_ex.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_rcc_ex.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_tim.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_tim_ex.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_uart.o \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_spi.o

C_DEPS += \
$(BuildPath)/driver/stdriver/stm32f7xx_hal.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_gpio.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_rcc.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_pwr.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_pwr_ex.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_cortex.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_dma.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_i2c.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_i2c_ex.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_rcc_ex.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_tim.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_tim_ex.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_uart.d \
$(BuildPath)/driver/stdriver/stm32f7xx_hal_spi.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/stdriver/%.o: ./driver/stdriver/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
