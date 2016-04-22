################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/test/arraytest.c \
../kern/test/automationtest.c \
../kern/test/bitmaptest.c \
../kern/test/fstest.c \
../kern/test/hmacunit.c \
../kern/test/kmalloctest.c \
../kern/test/lib.c \
../kern/test/nettest.c \
../kern/test/rwtest.c \
../kern/test/semunit.c \
../kern/test/synchprobs.c \
../kern/test/synchtest.c \
../kern/test/threadlisttest.c \
../kern/test/threadtest.c \
../kern/test/tt3.c 

OBJS += \
./kern/test/arraytest.o \
./kern/test/automationtest.o \
./kern/test/bitmaptest.o \
./kern/test/fstest.o \
./kern/test/hmacunit.o \
./kern/test/kmalloctest.o \
./kern/test/lib.o \
./kern/test/nettest.o \
./kern/test/rwtest.o \
./kern/test/semunit.o \
./kern/test/synchprobs.o \
./kern/test/synchtest.o \
./kern/test/threadlisttest.o \
./kern/test/threadtest.o \
./kern/test/tt3.o 

C_DEPS += \
./kern/test/arraytest.d \
./kern/test/automationtest.d \
./kern/test/bitmaptest.d \
./kern/test/fstest.d \
./kern/test/hmacunit.d \
./kern/test/kmalloctest.d \
./kern/test/lib.d \
./kern/test/nettest.d \
./kern/test/rwtest.d \
./kern/test/semunit.d \
./kern/test/synchprobs.d \
./kern/test/synchtest.d \
./kern/test/threadlisttest.d \
./kern/test/threadtest.d \
./kern/test/tt3.d 


# Each subdirectory must supply rules for building sources it contributes
kern/test/%.o: ../kern/test/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


