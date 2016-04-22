################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/matmult/matmult-orig.c \
../userland/testbin/matmult/matmult.c 

OBJS += \
./userland/testbin/matmult/matmult-orig.o \
./userland/testbin/matmult/matmult.o 

C_DEPS += \
./userland/testbin/matmult/matmult-orig.d \
./userland/testbin/matmult/matmult.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/matmult/%.o: ../userland/testbin/matmult/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


