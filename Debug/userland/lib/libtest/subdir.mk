################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libtest/quint.c \
../userland/lib/libtest/triple.c 

OBJS += \
./userland/lib/libtest/quint.o \
./userland/lib/libtest/triple.o 

C_DEPS += \
./userland/lib/libtest/quint.d \
./userland/lib/libtest/triple.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libtest/%.o: ../userland/lib/libtest/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


