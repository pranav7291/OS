################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/mv/mv.c 

OBJS += \
./userland/bin/mv/mv.o 

C_DEPS += \
./userland/bin/mv/mv.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/mv/%.o: ../userland/bin/mv/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


