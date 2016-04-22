################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/sbin/reboot/reboot.c 

OBJS += \
./userland/sbin/reboot/reboot.o 

C_DEPS += \
./userland/sbin/reboot/reboot.d 


# Each subdirectory must supply rules for building sources it contributes
userland/sbin/reboot/%.o: ../userland/sbin/reboot/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


