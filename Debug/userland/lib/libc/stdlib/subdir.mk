################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libc/stdlib/abort.c \
../userland/lib/libc/stdlib/exit.c \
../userland/lib/libc/stdlib/getenv.c \
../userland/lib/libc/stdlib/malloc.c \
../userland/lib/libc/stdlib/qsort.c \
../userland/lib/libc/stdlib/random.c \
../userland/lib/libc/stdlib/system.c 

OBJS += \
./userland/lib/libc/stdlib/abort.o \
./userland/lib/libc/stdlib/exit.o \
./userland/lib/libc/stdlib/getenv.o \
./userland/lib/libc/stdlib/malloc.o \
./userland/lib/libc/stdlib/qsort.o \
./userland/lib/libc/stdlib/random.o \
./userland/lib/libc/stdlib/system.o 

C_DEPS += \
./userland/lib/libc/stdlib/abort.d \
./userland/lib/libc/stdlib/exit.d \
./userland/lib/libc/stdlib/getenv.d \
./userland/lib/libc/stdlib/malloc.d \
./userland/lib/libc/stdlib/qsort.d \
./userland/lib/libc/stdlib/random.d \
./userland/lib/libc/stdlib/system.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/stdlib/%.o: ../userland/lib/libc/stdlib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


