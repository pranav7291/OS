################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/quinthuge/quinthuge.c 

OBJS += \
./userland/testbin/quinthuge/quinthuge.o 

C_DEPS += \
./userland/testbin/quinthuge/quinthuge.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/quinthuge/%.o: ../userland/testbin/quinthuge/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


