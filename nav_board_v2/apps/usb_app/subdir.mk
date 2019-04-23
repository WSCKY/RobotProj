################################################################################
# subdir Makefile
# kyChu@2019-4-22
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/usb_app/usb_bsp.c \
./apps/usb_app/usbd_cdc_vcp.c \
./apps/usb_app/usbd_desc.c \
./apps/usb_app/usbd_usr.c

OBJS += \
$(BuildPath)/apps/usb_app/usb_bsp.o \
$(BuildPath)/apps/usb_app/usbd_cdc_vcp.o \
$(BuildPath)/apps/usb_app/usbd_desc.o \
$(BuildPath)/apps/usb_app/usbd_usr.o

C_DEPS += \
$(BuildPath)/apps/usb_app/usb_bsp.d \
$(BuildPath)/apps/usb_app/usbd_cdc_vcp.d \
$(BuildPath)/apps/usb_app/usbd_desc.d \
$(BuildPath)/apps/usb_app/usbd_usr.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/usb_app/%.o: ./apps/usb_app/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
