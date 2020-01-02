################################################################################
# bsp/subdir.mk
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
./driver/bsp/src/gpio_irq.c \
./driver/bsp/src/rtcmif.c \
./driver/bsp/src/timebase.c \
./driver/bsp/src/ubloxa.c \
./driver/bsp/src/ubloxb.c \
./driver/bsp/src/leds.c \
./driver/bsp/src/imuif.c \
./driver/bsp/src/magif.c \
./driver/bsp/src/flashif.c \
./driver/bsp/src/usbdif.c \
./driver/bsp/src/ist83xx.c \
./driver/bsp/src/icm42605.c \
./driver/bsp/src/w25qxx.c \
./driver/bsp/src/usb_cdcif.c \
./driver/bsp/src/w25_diskio.c

OBJS += \
$(BuildPath)/driver/bsp/callback.o \
$(BuildPath)/driver/bsp/comif.o \
$(BuildPath)/driver/bsp/ec20.o \
$(BuildPath)/driver/bsp/hal_msp.o \
$(BuildPath)/driver/bsp/mcu_irq.o \
$(BuildPath)/driver/bsp/gpio_irq.o \
$(BuildPath)/driver/bsp/rtcmif.o \
$(BuildPath)/driver/bsp/timebase.o \
$(BuildPath)/driver/bsp/ubloxa.o \
$(BuildPath)/driver/bsp/ubloxb.o \
$(BuildPath)/driver/bsp/leds.o \
$(BuildPath)/driver/bsp/imuif.o \
$(BuildPath)/driver/bsp/magif.o \
$(BuildPath)/driver/bsp/flashif.o \
$(BuildPath)/driver/bsp/usbdif.o \
$(BuildPath)/driver/bsp/ist83xx.o \
$(BuildPath)/driver/bsp/icm42605.o \
$(BuildPath)/driver/bsp/w25qxx.o \
$(BuildPath)/driver/bsp/usb_cdcif.o \
$(BuildPath)/driver/bsp/w25_diskio.o

C_DEPS += \
$(BuildPath)/driver/bsp/callback.d \
$(BuildPath)/driver/bsp/comif.d \
$(BuildPath)/driver/bsp/ec20.d \
$(BuildPath)/driver/bsp/hal_msp.d \
$(BuildPath)/driver/bsp/mcu_irq.d \
$(BuildPath)/driver/bsp/gpio_irq.d \
$(BuildPath)/driver/bsp/rtcmif.d \
$(BuildPath)/driver/bsp/timebase.d \
$(BuildPath)/driver/bsp/ubloxa.d \
$(BuildPath)/driver/bsp/ubloxb.d \
$(BuildPath)/driver/bsp/leds.d \
$(BuildPath)/driver/bsp/imuif.d \
$(BuildPath)/driver/bsp/magif.d \
$(BuildPath)/driver/bsp/flashif.d \
$(BuildPath)/driver/bsp/usbdif.d \
$(BuildPath)/driver/bsp/ist83xx.d \
$(BuildPath)/driver/bsp/icm42605.d \
$(BuildPath)/driver/bsp/w25qxx.d \
$(BuildPath)/driver/bsp/usb_cdcif.d \
$(BuildPath)/driver/bsp/w25_diskio.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/bsp/%.o: ./driver/bsp/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
