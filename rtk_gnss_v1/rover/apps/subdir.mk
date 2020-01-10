################################################################################
# apps/subdir.mk
# Author: kyChu <kyChu@qq.com>
# Date:   2019-12-3
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./apps/src/apps.c \
./apps/src/f9pconfig.c \
./apps/src/gnss.c \
./apps/src/server.c \
./apps/src/compass.c \
./apps/src/test_case.c \
./apps/src/att_est_q.c \
./apps/src/cpu_utils.c \
./apps/src/filesystem.c \
./apps/src/filetransfer.c \
./apps/src/mesg.c

OBJS += \
$(BuildPath)/apps/apps.o \
$(BuildPath)/apps/f9pconfig.o \
$(BuildPath)/apps/gnss.o \
$(BuildPath)/apps/server.o \
$(BuildPath)/apps/compass.o \
$(BuildPath)/apps/test_case.o \
$(BuildPath)/apps/att_est_q.o \
$(BuildPath)/apps/cpu_utils.o \
$(BuildPath)/apps/filesystem.o \
$(BuildPath)/apps/filetransfer.o \
$(BuildPath)/apps/mesg.o

C_DEPS += \
$(BuildPath)/apps/apps.d \
$(BuildPath)/apps/f9pconfig.d \
$(BuildPath)/apps/gnss.d \
$(BuildPath)/apps/server.d \
$(BuildPath)/apps/compass.d \
$(BuildPath)/apps/test_case.d \
$(BuildPath)/apps/att_est_q.d \
$(BuildPath)/apps/cpu_utils.d \
$(BuildPath)/apps/filesystem.d \
$(BuildPath)/apps/filetransfer.d \
$(BuildPath)/apps/mesg.d

OBJ_DIRS = $(sort $(dir $(OBJS)))

# Each subdirectory must supply rules for building sources it contributes
$(BuildPath)/apps/%.o: ./apps/src/%.c | $(OBJ_DIRS)
	$(ECHO) ' CC $<'
	$(CC) $(PLATFORM) $(DEFS) $(INCS) $(CFGS) -O0 $(DBGS) -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
