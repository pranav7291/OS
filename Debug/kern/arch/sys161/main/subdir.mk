################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../kern/arch/sys161/main/start.S 

OBJS += \
./kern/arch/sys161/main/start.o 


# Each subdirectory must supply rules for building sources it contributes
kern/arch/sys161/main/%.o: ../kern/arch/sys161/main/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


