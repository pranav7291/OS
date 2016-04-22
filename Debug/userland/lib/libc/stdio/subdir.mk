################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libc/stdio/__puts.c \
../userland/lib/libc/stdio/getchar.c \
../userland/lib/libc/stdio/printf.c \
../userland/lib/libc/stdio/putchar.c \
../userland/lib/libc/stdio/puts.c 

OBJS += \
./userland/lib/libc/stdio/__puts.o \
./userland/lib/libc/stdio/getchar.o \
./userland/lib/libc/stdio/printf.o \
./userland/lib/libc/stdio/putchar.o \
./userland/lib/libc/stdio/puts.o 

C_DEPS += \
./userland/lib/libc/stdio/__puts.d \
./userland/lib/libc/stdio/getchar.d \
./userland/lib/libc/stdio/printf.d \
./userland/lib/libc/stdio/putchar.d \
./userland/lib/libc/stdio/puts.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/stdio/%.o: ../userland/lib/libc/stdio/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


