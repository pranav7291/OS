################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/testbin/frack/check.c \
../userland/testbin/frack/data.c \
../userland/testbin/frack/do.c \
../userland/testbin/frack/main.c \
../userland/testbin/frack/name.c \
../userland/testbin/frack/ops.c \
../userland/testbin/frack/pool.c \
../userland/testbin/frack/workloads.c 

OBJS += \
./userland/testbin/frack/check.o \
./userland/testbin/frack/data.o \
./userland/testbin/frack/do.o \
./userland/testbin/frack/main.o \
./userland/testbin/frack/name.o \
./userland/testbin/frack/ops.o \
./userland/testbin/frack/pool.o \
./userland/testbin/frack/workloads.o 

C_DEPS += \
./userland/testbin/frack/check.d \
./userland/testbin/frack/data.d \
./userland/testbin/frack/do.d \
./userland/testbin/frack/main.d \
./userland/testbin/frack/name.d \
./userland/testbin/frack/ops.d \
./userland/testbin/frack/pool.d \
./userland/testbin/frack/workloads.d 


# Each subdirectory must supply rules for building sources it contributes
userland/testbin/frack/%.o: ../userland/testbin/frack/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


