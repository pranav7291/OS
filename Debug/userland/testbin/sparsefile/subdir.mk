################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/sparsefile/sparsefile.c 

OBJS += \
./userland/testbin/sparsefile/sparsefile.o 

C_DEPS += \
./userland/testbin/sparsefile/sparsefile.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/sparsefile/%.o: ../userland/testbin/sparsefile/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


