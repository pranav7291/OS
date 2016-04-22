################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/ls/ls.c 

OBJS += \
./userland/bin/ls/ls.o 

C_DEPS += \
./userland/bin/ls/ls.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/ls/%.o: ../userland/bin/ls/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


