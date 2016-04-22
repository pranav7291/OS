################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/fs/semfs/semfs_fsops.c \
../kern/fs/semfs/semfs_obj.c \
../kern/fs/semfs/semfs_vnops.c 

OBJS += \
./kern/fs/semfs/semfs_fsops.o \
./kern/fs/semfs/semfs_obj.o \
./kern/fs/semfs/semfs_vnops.o 

C_DEPS += \
./kern/fs/semfs/semfs_fsops.d \
./kern/fs/semfs/semfs_obj.d \
./kern/fs/semfs/semfs_vnops.d 


# Each subdirectory must supply rules for building sources it contributes
kern/fs/semfs/%.o: ../kern/fs/semfs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


