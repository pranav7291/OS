################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/sbin/mksfs/disk.c \
../userland/sbin/mksfs/mksfs.c \
../userland/sbin/mksfs/support.c 

OBJS += \
./userland/sbin/mksfs/disk.o \
./userland/sbin/mksfs/mksfs.o \
./userland/sbin/mksfs/support.o 

C_DEPS += \
./userland/sbin/mksfs/disk.d \
./userland/sbin/mksfs/mksfs.d \
./userland/sbin/mksfs/support.d 


# Each subdirectory must supply rules for building sources it contributes
userland/sbin/mksfs/%.o: ../userland/sbin/mksfs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


