################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/libc/unix/__assert.c \
../userland/lib/libc/unix/err.c \
../userland/lib/libc/unix/errno.c \
../userland/lib/libc/unix/execvp.c \
../userland/lib/libc/unix/getcwd.c 

OBJS += \
./userland/lib/libc/unix/__assert.o \
./userland/lib/libc/unix/err.o \
./userland/lib/libc/unix/errno.o \
./userland/lib/libc/unix/execvp.o \
./userland/lib/libc/unix/getcwd.o 

C_DEPS += \
./userland/lib/libc/unix/__assert.d \
./userland/lib/libc/unix/err.d \
./userland/lib/libc/unix/errno.d \
./userland/lib/libc/unix/execvp.d \
./userland/lib/libc/unix/getcwd.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/libc/unix/%.o: ../userland/lib/libc/unix/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


