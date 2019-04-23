################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/usb_otg/src/usb_core.c \
./driver/usb_otg/src/usb_dcd.c \
./driver/usb_otg/src/usb_dcd_int.c

OBJS += \
$(BuildPath)/driver/usb_otg/usb_core.o \
$(BuildPath)/driver/usb_otg/usb_dcd.o \
$(BuildPath)/driver/usb_otg/usb_dcd_int.o

C_DEPS += \
$(BuildPath)/driver/usb_otg/usb_core.d \
$(BuildPath)/driver/usb_otg/usb_dcd.d \
$(BuildPath)/driver/usb_otg/usb_dcd_int.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/usb_otg/%.o: ./driver/usb_otg/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
