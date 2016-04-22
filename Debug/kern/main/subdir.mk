################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/main/main.c \
../kern/main/menu.c 

OBJS += \
./kern/main/main.o \
./kern/main/menu.o 

C_DEPS += \
./kern/main/main.d \
./kern/main/menu.d 


# Each subdirectory must supply rules for building sources it contributes
kern/main/%.o: ../kern/main/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


