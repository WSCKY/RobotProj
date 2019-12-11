################################################################################
# freertos/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/freertos/src/croutine.c \
./middleware/freertos/src/cmsis_os.c \
./middleware/freertos/src/event_groups.c \
./middleware/freertos/src/heap_4.c \
./middleware/freertos/src/list.c \
./middleware/freertos/src/port.c \
./middleware/freertos/src/queue.c \
./middleware/freertos/src/tasks.c \
./middleware/freertos/src/timers.c

OBJS += \
$(BuildPath)/middleware/freertos/croutine.o \
$(BuildPath)/middleware/freertos/cmsis_os.o \
$(BuildPath)/middleware/freertos/event_groups.o \
$(BuildPath)/middleware/freertos/heap_4.o \
$(BuildPath)/middleware/freertos/list.o \
$(BuildPath)/middleware/freertos/port.o \
$(BuildPath)/middleware/freertos/queue.o \
$(BuildPath)/middleware/freertos/tasks.o \
$(BuildPath)/middleware/freertos/timers.o

C_DEPS += \
$(BuildPath)/middleware/freertos/croutine.d \
$(BuildPath)/middleware/freertos/cmsis_os.d \
$(BuildPath)/middleware/freertos/event_groups.d \
$(BuildPath)/middleware/freertos/heap_4.d \
$(BuildPath)/middleware/freertos/list.d \
$(BuildPath)/middleware/freertos/port.d \
$(BuildPath)/middleware/freertos/queue.d \
$(BuildPath)/middleware/freertos/tasks.d \
$(BuildPath)/middleware/freertos/timers.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/freertos/%.o: ./middleware/freertos/src/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
