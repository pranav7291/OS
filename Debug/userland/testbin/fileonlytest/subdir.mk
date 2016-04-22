################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/fileonlytest/fileonlytest.c 

OBJS += \
./userland/testbin/fileonlytest/fileonlytest.o 

C_DEPS += \
./userland/testbin/fileonlytest/fileonlytest.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/fileonlytest/%.o: ../userland/testbin/fileonlytest/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


