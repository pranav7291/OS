################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libc/time/time.c 

OBJS += \
./userland/lib/libc/time/time.o 

C_DEPS += \
./userland/lib/libc/time/time.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/time/%.o: ../userland/lib/libc/time/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


