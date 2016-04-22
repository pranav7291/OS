################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/arch/mips/vm/dumbvm.c \
../kern/arch/mips/vm/ram.c 

S_UPPER_SRCS += \
../kern/arch/mips/vm/tlb-mips161.S 

OBJS += \
./kern/arch/mips/vm/dumbvm.o \
./kern/arch/mips/vm/ram.o \
./kern/arch/mips/vm/tlb-mips161.o 

C_DEPS += \
./kern/arch/mips/vm/dumbvm.d \
./kern/arch/mips/vm/ram.d 


# Each subdirectory must supply rules for building sources it contributes
kern/arch/mips/vm/%.o: ../kern/arch/mips/vm/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

kern/arch/mips/vm/%.o: ../kern/arch/mips/vm/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	as  -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


