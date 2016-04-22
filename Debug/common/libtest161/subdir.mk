################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../common/libtest161/secure.c \
../common/libtest161/sha256.c \
../common/libtest161/test161.c 

OBJS += \
./common/libtest161/secure.o \
./common/libtest161/sha256.o \
./common/libtest161/test161.o 

C_DEPS += \
./common/libtest161/secure.d \
./common/libtest161/sha256.d \
./common/libtest161/test161.d 


# Each subdirectory must supply rules for building sources it contributes
common/libtest161/%.o: ../common/libtest161/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


