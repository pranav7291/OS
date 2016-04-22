################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/filetest/filetest.c 

OBJS += \
./userland/testbin/filetest/filetest.o 

C_DEPS += \
./userland/testbin/filetest/filetest.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/filetest/%.o: ../userland/testbin/filetest/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


