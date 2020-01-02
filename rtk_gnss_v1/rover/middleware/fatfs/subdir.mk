################################################################################
# fatfs/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-27
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./middleware/fatfs/ff.c \
./middleware/fatfs/diskio.c \
./middleware/fatfs/ff_gen_drv.c \
./middleware/fatfs/option/syscall.c \
./middleware/fatfs/option/unicode.c

OBJS += \
$(BuildPath)/middleware/fatfs/ff.o \
$(BuildPath)/middleware/fatfs/diskio.o \
$(BuildPath)/middleware/fatfs/ff_gen_drv.o \
$(BuildPath)/middleware/fatfs/option/syscall.o \
$(BuildPath)/middleware/fatfs/option/unicode.o

C_DEPS += \
$(BuildPath)/middleware/fatfs/ff.d \
$(BuildPath)/middleware/fatfs/diskio.d \
$(BuildPath)/middleware/fatfs/ff_gen_drv.d \
$(BuildPath)/middleware/fatfs/option/syscall.d \
$(BuildPath)/middleware/fatfs/option/unicode.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/middleware/fatfs/%.o: ./middleware/fatfs/%.c | $(OBJ_DIRS)
	@echo ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -Os $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
