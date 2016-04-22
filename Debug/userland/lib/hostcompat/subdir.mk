################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../userland/lib/hostcompat/err.c \
../userland/lib/hostcompat/hostcompat.c \
../userland/lib/hostcompat/ntohll.c \
../userland/lib/hostcompat/time.c 

OBJS += \
./userland/lib/hostcompat/err.o \
./userland/lib/hostcompat/hostcompat.o \
./userland/lib/hostcompat/ntohll.o \
./userland/lib/hostcompat/time.o 

C_DEPS += \
./userland/lib/hostcompat/err.d \
./userland/lib/hostcompat/hostcompat.d \
./userland/lib/hostcompat/ntohll.d \
./userland/lib/hostcompat/time.d 


# Each subdirectory must supply rules for building sources it contributes
userland/lib/hostcompat/%.o: ../userland/lib/hostcompat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


