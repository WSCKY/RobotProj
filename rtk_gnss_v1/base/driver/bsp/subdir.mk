################################################################################
# driver/bsp/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/bsp/src/callback.c \
./driver/bsp/src/comif.c \
./driver/bsp/src/ec20.c \
./driver/bsp/src/hal_msp.c \
./driver/bsp/src/mcu_irq.c \
./driver/bsp/src/rtcmif.c \
./driver/bsp/src/timebase_tim.c \
./driver/bsp/src/ubloxa.c \
./driver/bsp/src/ubloxb.c \
./driver/bsp/src/stm32f7xx_rtk_gps_nav.c \
./driver/bsp/src/imuif.c \
./driver/bsp/src/icm42605.c

OBJS += \
$(BuildPath)/driver/bsp/callback.o \
$(BuildPath)/driver/bsp/comif.o \
$(BuildPath)/driver/bsp/ec20.o \
$(BuildPath)/driver/bsp/hal_msp.o \
$(BuildPath)/driver/bsp/mcu_irq.o \
$(BuildPath)/driver/bsp/rtcmif.o \
$(BuildPath)/driver/bsp/timebase_tim.o \
$(BuildPath)/driver/bsp/ubloxa.o \
$(BuildPath)/driver/bsp/ubloxb.o \
$(BuildPath)/driver/bsp/stm32f7xx_rtk_gps_nav.o \
$(BuildPath)/driver/bsp/imuif.o \
$(BuildPath)/driver/bsp/icm42605.o

C_DEPS += \
$(BuildPath)/driver/bsp/callback.d \
$(BuildPath)/driver/bsp/comif.d \
$(BuildPath)/driver/bsp/ec20.d \
$(BuildPath)/driver/bsp/hal_msp.d \
$(BuildPath)/driver/bsp/mcu_irq.d \
$(BuildPath)/driver/bsp/rtcmif.d \
$(BuildPath)/driver/bsp/timebase_tim.d \
$(BuildPath)/driver/bsp/ubloxa.d \
$(BuildPath)/driver/bsp/ubloxb.d \
$(BuildPath)/driver/bsp/stm32f7xx_rtk_gps_nav.d \
$(BuildPath)/driver/bsp/imuif.d \
$(BuildPath)/driver/bsp/icm42605.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/bsp/%.o: ./driver/bsp/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
