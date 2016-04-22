################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libc/string/memcmp.c \
../userland/lib/libc/string/strerror.c \
../userland/lib/libc/string/strtok.c 

OBJS += \
./userland/lib/libc/string/memcmp.o \
./userland/lib/libc/string/strerror.o \
./userland/lib/libc/string/strtok.o 

C_DEPS += \
./userland/lib/libc/string/memcmp.d \
./userland/lib/libc/string/strerror.d \
./userland/lib/libc/string/strtok.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/string/%.o: ../userland/lib/libc/string/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


