################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/bin/rm/rm.c 

OBJS += \
./userland/bin/rm/rm.o 

C_DEPS += \
./userland/bin/rm/rm.d 


# Each subdirectory must supply rules for building sources it contributes
userland/bin/rm/%.o: ../userland/bin/rm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


