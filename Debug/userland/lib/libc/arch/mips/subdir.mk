################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../userland/lib/libc/arch/mips/syscalls-mips.S 

OBJS += \
./userland/lib/libc/arch/mips/syscalls-mips.o 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/arch/mips/%.o: ../userland/lib/libc/arch/mips/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


