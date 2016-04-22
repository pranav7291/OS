################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/mkdir/mkdir.c 

OBJS += \
./userland/bin/mkdir/mkdir.o 

C_DEPS += \
./userland/bin/mkdir/mkdir.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/mkdir/%.o: ../userland/bin/mkdir/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


