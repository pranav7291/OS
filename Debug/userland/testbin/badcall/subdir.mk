################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/badcall/bad_chdir.c \
../userland/testbin/badcall/bad_close.c \
../userland/testbin/badcall/bad_dup2.c \
../userland/testbin/badcall/bad_execv.c \
../userland/testbin/badcall/bad_fsync.c \
../userland/testbin/badcall/bad_ftruncate.c \
../userland/testbin/badcall/bad_getcwd.c \
../userland/testbin/badcall/bad_getdirentry.c \
../userland/testbin/badcall/bad_ioctl.c \
../userland/testbin/badcall/bad_link.c \
../userland/testbin/badcall/bad_lseek.c \
../userland/testbin/badcall/bad_mkdir.c \
../userland/testbin/badcall/bad_open.c \
../userland/testbin/badcall/bad_pipe.c \
../userland/testbin/badcall/bad_read.c \
../userland/testbin/badcall/bad_readlink.c \
../userland/testbin/badcall/bad_reboot.c \
../userland/testbin/badcall/bad_remove.c \
../userland/testbin/badcall/bad_rename.c \
../userland/testbin/badcall/bad_rmdir.c \
../userland/testbin/badcall/bad_sbrk.c \
../userland/testbin/badcall/bad_stat.c \
../userland/testbin/badcall/bad_symlink.c \
../userland/testbin/badcall/bad_time.c \
../userland/testbin/badcall/bad_waitpid.c \
../userland/testbin/badcall/bad_write.c \
../userland/testbin/badcall/common_buf.c \
../userland/testbin/badcall/common_fds.c \
../userland/testbin/badcall/common_path.c \
../userland/testbin/badcall/driver.c \
../userland/testbin/badcall/report.c 

OBJS += \
./userland/testbin/badcall/bad_chdir.o \
./userland/testbin/badcall/bad_close.o \
./userland/testbin/badcall/bad_dup2.o \
./userland/testbin/badcall/bad_execv.o \
./userland/testbin/badcall/bad_fsync.o \
./userland/testbin/badcall/bad_ftruncate.o \
./userland/testbin/badcall/bad_getcwd.o \
./userland/testbin/badcall/bad_getdirentry.o \
./userland/testbin/badcall/bad_ioctl.o \
./userland/testbin/badcall/bad_link.o \
./userland/testbin/badcall/bad_lseek.o \
./userland/testbin/badcall/bad_mkdir.o \
./userland/testbin/badcall/bad_open.o \
./userland/testbin/badcall/bad_pipe.o \
./userland/testbin/badcall/bad_read.o \
./userland/testbin/badcall/bad_readlink.o \
./userland/testbin/badcall/bad_reboot.o \
./userland/testbin/badcall/bad_remove.o \
./userland/testbin/badcall/bad_rename.o \
./userland/testbin/badcall/bad_rmdir.o \
./userland/testbin/badcall/bad_sbrk.o \
./userland/testbin/badcall/bad_stat.o \
./userland/testbin/badcall/bad_symlink.o \
./userland/testbin/badcall/bad_time.o \
./userland/testbin/badcall/bad_waitpid.o \
./userland/testbin/badcall/bad_write.o \
./userland/testbin/badcall/common_buf.o \
./userland/testbin/badcall/common_fds.o \
./userland/testbin/badcall/common_path.o \
./userland/testbin/badcall/driver.o \
./userland/testbin/badcall/report.o 

C_DEPS += \
./userland/testbin/badcall/bad_chdir.d \
./userland/testbin/badcall/bad_close.d \
./userland/testbin/badcall/bad_dup2.d \
./userland/testbin/badcall/bad_execv.d \
./userland/testbin/badcall/bad_fsync.d \
./userland/testbin/badcall/bad_ftruncate.d \
./userland/testbin/badcall/bad_getcwd.d \
./userland/testbin/badcall/bad_getdirentry.d \
./userland/testbin/badcall/bad_ioctl.d \
./userland/testbin/badcall/bad_link.d \
./userland/testbin/badcall/bad_lseek.d \
./userland/testbin/badcall/bad_mkdir.d \
./userland/testbin/badcall/bad_open.d \
./userland/testbin/badcall/bad_pipe.d \
./userland/testbin/badcall/bad_read.d \
./userland/testbin/badcall/bad_readlink.d \
./userland/testbin/badcall/bad_reboot.d \
./userland/testbin/badcall/bad_remove.d \
./userland/testbin/badcall/bad_rename.d \
./userland/testbin/badcall/bad_rmdir.d \
./userland/testbin/badcall/bad_sbrk.d \
./userland/testbin/badcall/bad_stat.d \
./userland/testbin/badcall/bad_symlink.d \
./userland/testbin/badcall/bad_time.d \
./userland/testbin/badcall/bad_waitpid.d \
./userland/testbin/badcall/bad_write.d \
./userland/testbin/badcall/common_buf.d \
./userland/testbin/badcall/common_fds.d \
./userland/testbin/badcall/common_path.d \
./userland/testbin/badcall/driver.d \
./userland/testbin/badcall/report.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/badcall/%.o: ../userland/testbin/badcall/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


