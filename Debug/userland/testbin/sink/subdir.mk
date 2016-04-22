################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/sink/sink.c 

OBJS += \
./userland/testbin/sink/sink.o 

C_DEPS += \
./userland/testbin/sink/sink.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/sink/%.o: ../userland/testbin/sink/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


