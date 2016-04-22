################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/synchprobs/stoplight.c \
../kern/synchprobs/whalemating.c 

OBJS += \
./kern/synchprobs/stoplight.o \
./kern/synchprobs/whalemating.o 

C_DEPS += \
./kern/synchprobs/stoplight.d \
./kern/synchprobs/whalemating.d 


# Each subdirectory must supply rules for building sources it contributes
kern/synchprobs/%.o: ../kern/synchprobs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


