################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/true/true.c 

OBJS += \
./userland/bin/true/true.o 

C_DEPS += \
./userland/bin/true/true.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/true/%.o: ../userland/bin/true/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


