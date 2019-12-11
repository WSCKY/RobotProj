################################################################################
# usbdrv/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
./driver/usbdrv/core/usbd_core.c \
./driver/usbdrv/core/usbd_ctlreq.c \
./driver/usbdrv/core/usbd_ioreq.c \
./driver/usbdrv/cdc/usbd_cdc.c \
./driver/usbdrv/cdc/usbd_desc.c

OBJS += \
$(BuildPath)/driver/usbdrv/core/usbd_core.o \
$(BuildPath)/driver/usbdrv/core/usbd_ctlreq.o \
$(BuildPath)/driver/usbdrv/core/usbd_ioreq.o \
$(BuildPath)/driver/usbdrv/cdc/usbd_cdc.o \
$(BuildPath)/driver/usbdrv/cdc/usbd_desc.o

C_DEPS += \
$(BuildPath)/driver/usbdrv/core/usbd_core.d \
$(BuildPath)/driver/usbdrv/core/usbd_ctlreq.d \
$(BuildPath)/driver/usbdrv/core/usbd_ioreq.d \
$(BuildPath)/driver/usbdrv/cdc/usbd_cdc.d \
$(BuildPath)/driver/usbdrv/cdc/usbd_desc.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/driver/usbdrv/cdc/%.o: ./driver/usbdrv/cdc/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"

$(BuildPath)/driver/usbdrv/core/%.o: ./driver/usbdrv/core/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
