################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/proc/proc.c 

OBJS += \
./kern/proc/proc.o 

C_DEPS += \
./kern/proc/proc.d 


# Each subdirectory must supply rules for building sources it contributes
kern/proc/%.o: ../kern/proc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


