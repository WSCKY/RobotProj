################################################################################
# subdir Makefile
# kyChu@2019-2-20
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/usb_cdc/src/usbd_cdc_core.c \
./driver/usb_cdc/src/usbd_core.c \
./driver/usb_cdc/src/usbd_ioreq.c \
./driver/usb_cdc/src/usbd_req.c

OBJS += \
$(BuildPath)/driver/usb_cdc/usbd_cdc_core.o \
$(BuildPath)/driver/usb_cdc/usbd_core.o \
$(BuildPath)/driver/usb_cdc/usbd_ioreq.o \
$(BuildPath)/driver/usb_cdc/usbd_req.o

C_DEPS += \
$(BuildPath)/driver/usb_cdc/usbd_cdc_core.d \
$(BuildPath)/driver/usb_cdc/usbd_core.d \
$(BuildPath)/driver/usb_cdc/usbd_ioreq.d \
$(BuildPath)/driver/usb_cdc/usbd_req.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/usb_cdc/%.o: ./driver/usb_cdc/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
