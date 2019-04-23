################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/bsp/src/AT24CXX.c \
./driver/bsp/src/Button.c \
./driver/bsp/src/ComPort.c \
./driver/bsp/src/Heater.c \
./driver/bsp/src/imu_spi.c \
./driver/bsp/src/LED.c \
./driver/bsp/src/MAX_M8Q.c \
./driver/bsp/src/MPU9250.c \
./driver/bsp/src/PwrCtrl.c \
./driver/bsp/src/TimerCounter.c \
./driver/bsp/src/ubxUART.c

OBJS += \
$(BuildPath)/driver/bsp/AT24CXX.o \
$(BuildPath)/driver/bsp/Button.o \
$(BuildPath)/driver/bsp/ComPort.o \
$(BuildPath)/driver/bsp/Heater.o \
$(BuildPath)/driver/bsp/imu_spi.o \
$(BuildPath)/driver/bsp/LED.o \
$(BuildPath)/driver/bsp/MAX_M8Q.o \
$(BuildPath)/driver/bsp/MPU9250.o \
$(BuildPath)/driver/bsp/PwrCtrl.o \
$(BuildPath)/driver/bsp/TimerCounter.o \
$(BuildPath)/driver/bsp/ubxUART.o

C_DEPS += \
$(BuildPath)/driver/bsp/AT24CXX.d \
$(BuildPath)/driver/bsp/Button.d \
$(BuildPath)/driver/bsp/ComPort.d \
$(BuildPath)/driver/bsp/Heater.d \
$(BuildPath)/driver/bsp/imu_spi.d \
$(BuildPath)/driver/bsp/LED.d \
$(BuildPath)/driver/bsp/MAX_M8Q.d \
$(BuildPath)/driver/bsp/MPU9250.d \
$(BuildPath)/driver/bsp/PwrCtrl.d \
$(BuildPath)/driver/bsp/TimerCounter.d \
$(BuildPath)/driver/bsp/ubxUART.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/bsp/%.o: ./driver/bsp/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
