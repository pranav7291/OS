################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/triplemat/triplemat.c 

OBJS += \
./userland/testbin/triplemat/triplemat.o 

C_DEPS += \
./userland/testbin/triplemat/triplemat.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/triplemat/%.o: ../userland/testbin/triplemat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


