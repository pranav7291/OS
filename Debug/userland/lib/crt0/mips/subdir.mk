################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../userland/lib/crt0/mips/crt0.S 

OBJS += \
./userland/lib/crt0/mips/crt0.o 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/crt0/mips/%.o: ../userland/lib/crt0/mips/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


