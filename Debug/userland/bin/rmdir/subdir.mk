################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/rmdir/rmdir.c 

OBJS += \
./userland/bin/rmdir/rmdir.o 

C_DEPS += \
./userland/bin/rmdir/rmdir.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/rmdir/%.o: ../userland/bin/rmdir/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


