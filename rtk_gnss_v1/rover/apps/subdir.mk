################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/apps.c \
./apps/f9pconfig.c \
./apps/gnss.c \
./apps/server.c \
./apps/test_case.c \
./apps/cpu_utils.c \
./apps/mesg.c

OBJS += \
$(BuildPath)/apps/apps.o \
$(BuildPath)/apps/f9pconfig.o \
$(BuildPath)/apps/gnss.o \
$(BuildPath)/apps/server.o \
$(BuildPath)/apps/test_case.o \
$(BuildPath)/apps/cpu_utils.o \
$(BuildPath)/apps/mesg.o

C_DEPS += \
$(BuildPath)/apps/apps.d \
$(BuildPath)/apps/f9pconfig.d \
$(BuildPath)/apps/gnss.d \
$(BuildPath)/apps/server.d \
$(BuildPath)/apps/test_case.d \
$(BuildPath)/apps/cpu_utils.d \
$(BuildPath)/apps/mesg.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
