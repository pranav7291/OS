################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../kern/fs/sfs/sfs_balloc.c \
../kern/fs/sfs/sfs_bmap.c \
../kern/fs/sfs/sfs_dir.c \
../kern/fs/sfs/sfs_fsops.c \
../kern/fs/sfs/sfs_inode.c \
../kern/fs/sfs/sfs_io.c \
../kern/fs/sfs/sfs_vnops.c 

OBJS += \
./kern/fs/sfs/sfs_balloc.o \
./kern/fs/sfs/sfs_bmap.o \
./kern/fs/sfs/sfs_dir.o \
./kern/fs/sfs/sfs_fsops.o \
./kern/fs/sfs/sfs_inode.o \
./kern/fs/sfs/sfs_io.o \
./kern/fs/sfs/sfs_vnops.o 

C_DEPS += \
./kern/fs/sfs/sfs_balloc.d \
./kern/fs/sfs/sfs_bmap.d \
./kern/fs/sfs/sfs_dir.d \
./kern/fs/sfs/sfs_fsops.d \
./kern/fs/sfs/sfs_inode.d \
./kern/fs/sfs/sfs_io.d \
./kern/fs/sfs/sfs_vnops.d 


# Each subdirectory must supply rules for building sources it contributes
kern/fs/sfs/%.o: ../kern/fs/sfs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


