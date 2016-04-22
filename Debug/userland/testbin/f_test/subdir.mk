################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/f_test/f_read.c \
../userland/testbin/f_test/f_test.c \
../userland/testbin/f_test/f_write.c 

OBJS += \
./userland/testbin/f_test/f_read.o \
./userland/testbin/f_test/f_test.o \
./userland/testbin/f_test/f_write.o 

C_DEPS += \
./userland/testbin/f_test/f_read.d \
./userland/testbin/f_test/f_test.d \
./userland/testbin/f_test/f_write.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/f_test/%.o: ../userland/testbin/f_test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


