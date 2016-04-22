################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/sbin/sfsck/freemap.c \
../userland/sbin/sfsck/inode.c \
../userland/sbin/sfsck/main.c \
../userland/sbin/sfsck/pass1.c \
../userland/sbin/sfsck/pass2.c \
../userland/sbin/sfsck/sb.c \
../userland/sbin/sfsck/sfs.c \
../userland/sbin/sfsck/utils.c 

OBJS += \
./userland/sbin/sfsck/freemap.o \
./userland/sbin/sfsck/inode.o \
./userland/sbin/sfsck/main.o \
./userland/sbin/sfsck/pass1.o \
./userland/sbin/sfsck/pass2.o \
./userland/sbin/sfsck/sb.o \
./userland/sbin/sfsck/sfs.o \
./userland/sbin/sfsck/utils.o 

C_DEPS += \
./userland/sbin/sfsck/freemap.d \
./userland/sbin/sfsck/inode.d \
./userland/sbin/sfsck/main.d \
./userland/sbin/sfsck/pass1.d \
./userland/sbin/sfsck/pass2.d \
./userland/sbin/sfsck/sb.d \
./userland/sbin/sfsck/sfs.d \
./userland/sbin/sfsck/utils.d 


# Each subdirectory must supply rules for building sources it contributes
userland/sbin/sfsck/%.o: ../userland/sbin/sfsck/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


