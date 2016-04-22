################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/sbin/dumpsfs/dumpsfs.c 

OBJS += \
./userland/sbin/dumpsfs/dumpsfs.o 

C_DEPS += \
./userland/sbin/dumpsfs/dumpsfs.d 


# Each subdirectory must supply rules for building sources it contributes
userland/sbin/dumpsfs/%.o: ../userland/sbin/dumpsfs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


