################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/dirconc/dirconc.c 

OBJS += \
./userland/testbin/dirconc/dirconc.o 

C_DEPS += \
./userland/testbin/dirconc/dirconc.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/dirconc/%.o: ../userland/testbin/dirconc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


