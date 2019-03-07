################################################################################
# subdir Makefile
# kyChu@2019-2-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./driver/usb/app/usb_bsp.c \
./driver/usb/app/usbd_usr.c \
./driver/usb/app/usbd_pwr.c \
./driver/usb/app/usbd_desc.c \
./driver/usb/app/usbd_cdc_vcp.c \
./driver/usb/drv/usb_core.c \
./driver/usb/drv/usb_dcd.c \
./driver/usb/drv/usb_dcd_int.c \
./driver/usb/lib/core/usbd_req.c \
./driver/usb/lib/core/usbd_core.c \
./driver/usb/lib/core/usbd_ioreq.c \
./driver/usb/lib/class/usbd_cdc_core.c

OBJS += \
$(BuildPath)/driver/usb/app/usb_bsp.o \
$(BuildPath)/driver/usb/app/usbd_usr.o \
$(BuildPath)/driver/usb/app/usbd_pwr.o \
$(BuildPath)/driver/usb/app/usbd_desc.o \
$(BuildPath)/driver/usb/app/usbd_cdc_vcp.o \
$(BuildPath)/driver/usb/drv/usb_core.o \
$(BuildPath)/driver/usb/drv/usb_dcd.o \
$(BuildPath)/driver/usb/drv/usb_dcd_int.o \
$(BuildPath)/driver/usb/lib/core/usbd_req.o \
$(BuildPath)/driver/usb/lib/core/usbd_core.o \
$(BuildPath)/driver/usb/lib/core/usbd_ioreq.o \
$(BuildPath)/driver/usb/lib/class/usbd_cdc_core.o

C_DEPS += \
$(BuildPath)/driver/usb/app/usb_bsp.d \
$(BuildPath)/driver/usb/app/usbd_usr.d \
$(BuildPath)/driver/usb/app/usbd_pwr.d \
$(BuildPath)/driver/usb/app/usbd_desc.d \
$(BuildPath)/driver/usb/app/usbd_cdc_vcp.d \
$(BuildPath)/driver/usb/drv/usb_core.d \
$(BuildPath)/driver/usb/drv/usb_dcd.d \
$(BuildPath)/driver/usb/drv/usb_dcd_int.d \
$(BuildPath)/driver/usb/lib/core/usbd_req.d \
$(BuildPath)/driver/usb/lib/core/usbd_core.d \
$(BuildPath)/driver/usb/lib/core/usbd_ioreq.d \
$(BuildPath)/driver/usb/lib/class/usbd_cdc_core.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/usb/%.o: ./driver/usb/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) -mcpu=cortex-m0 -mthumb -mfloat-abi=soft $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
