################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/bsp/src/bsp.c \
./driver/bsp/src/irq.c \
./driver/bsp/src/gpio.c \
./driver/bsp/src/button.c \
./driver/bsp/src/com_port.c \
./driver/bsp/src/Heater.c \
./driver/bsp/src/imu_spi.c \
./driver/bsp/src/usb_bsp.c \
./driver/bsp/src/color_led.c \
./driver/bsp/src/MAX_M8Q.c \
./driver/bsp/src/lsm9ds1.c \
./driver/bsp/src/TimerCounter.c \
./driver/bsp/src/ubxUART.c

OBJS += \
$(BuildPath)/driver/bsp/bsp.o \
$(BuildPath)/driver/bsp/irq.o \
$(BuildPath)/driver/bsp/gpio.o \
$(BuildPath)/driver/bsp/button.o \
$(BuildPath)/driver/bsp/com_port.o \
$(BuildPath)/driver/bsp/Heater.o \
$(BuildPath)/driver/bsp/imu_spi.o \
$(BuildPath)/driver/bsp/usb_bsp.o \
$(BuildPath)/driver/bsp/color_led.o \
$(BuildPath)/driver/bsp/MAX_M8Q.o \
$(BuildPath)/driver/bsp/lsm9ds1.o \
$(BuildPath)/driver/bsp/TimerCounter.o \
$(BuildPath)/driver/bsp/ubxUART.o

C_DEPS += \
$(BuildPath)/driver/bsp/bsp.d \
$(BuildPath)/driver/bsp/irq.d \
$(BuildPath)/driver/bsp/gpio.d \
$(BuildPath)/driver/bsp/button.d \
$(BuildPath)/driver/bsp/com_port.d \
$(BuildPath)/driver/bsp/Heater.d \
$(BuildPath)/driver/bsp/imu_spi.d \
$(BuildPath)/driver/bsp/usb_bsp.d \
$(BuildPath)/driver/bsp/color_led.d \
$(BuildPath)/driver/bsp/MAX_M8Q.d \
$(BuildPath)/driver/bsp/lsm9ds1.d \
$(BuildPath)/driver/bsp/TimerCounter.d \
$(BuildPath)/driver/bsp/ubxUART.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/bsp/%.o: ./driver/bsp/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
