################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/libc/printf/__printf.c \
../common/libc/printf/snprintf.c \
../common/libc/printf/tprintf.c 

OBJS += \
./common/libc/printf/__printf.o \
./common/libc/printf/snprintf.o \
./common/libc/printf/tprintf.o 

C_DEPS += \
./common/libc/printf/__printf.d \
./common/libc/printf/snprintf.d \
./common/libc/printf/tprintf.d 


# Each subdirectory must supply rules for building sources it contributes
common/libc/printf/%.o: ../common/libc/printf/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


