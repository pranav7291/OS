################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/schedpong/grind.c \
../userland/testbin/schedpong/main.c \
../userland/testbin/schedpong/pong.c \
../userland/testbin/schedpong/results.c \
../userland/testbin/schedpong/think.c \
../userland/testbin/schedpong/usem.c 

OBJS += \
./userland/testbin/schedpong/grind.o \
./userland/testbin/schedpong/main.o \
./userland/testbin/schedpong/pong.o \
./userland/testbin/schedpong/results.o \
./userland/testbin/schedpong/think.o \
./userland/testbin/schedpong/usem.o 

C_DEPS += \
./userland/testbin/schedpong/grind.d \
./userland/testbin/schedpong/main.d \
./userland/testbin/schedpong/pong.d \
./userland/testbin/schedpong/results.d \
./userland/testbin/schedpong/think.d \
./userland/testbin/schedpong/usem.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/schedpong/%.o: ../userland/testbin/schedpong/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


