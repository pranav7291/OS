################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/bloat/bloat.c 

OBJS += \
./userland/testbin/bloat/bloat.o 

C_DEPS += \
./userland/testbin/bloat/bloat.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/bloat/%.o: ../userland/testbin/bloat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


