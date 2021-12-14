################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus_master_slave/functions/mbfunccoils.c \
../modbus_master_slave/functions/mbfunccoils_m.c \
../modbus_master_slave/functions/mbfuncdiag.c \
../modbus_master_slave/functions/mbfuncdisc.c \
../modbus_master_slave/functions/mbfuncdisc_m.c \
../modbus_master_slave/functions/mbfuncholding.c \
../modbus_master_slave/functions/mbfuncholding_m.c \
../modbus_master_slave/functions/mbfuncinput.c \
../modbus_master_slave/functions/mbfuncinput_m.c \
../modbus_master_slave/functions/mbfuncother.c \
../modbus_master_slave/functions/mbutils.c 

OBJS += \
./modbus_master_slave/functions/mbfunccoils.o \
./modbus_master_slave/functions/mbfunccoils_m.o \
./modbus_master_slave/functions/mbfuncdiag.o \
./modbus_master_slave/functions/mbfuncdisc.o \
./modbus_master_slave/functions/mbfuncdisc_m.o \
./modbus_master_slave/functions/mbfuncholding.o \
./modbus_master_slave/functions/mbfuncholding_m.o \
./modbus_master_slave/functions/mbfuncinput.o \
./modbus_master_slave/functions/mbfuncinput_m.o \
./modbus_master_slave/functions/mbfuncother.o \
./modbus_master_slave/functions/mbutils.o 

C_DEPS += \
./modbus_master_slave/functions/mbfunccoils.d \
./modbus_master_slave/functions/mbfunccoils_m.d \
./modbus_master_slave/functions/mbfuncdiag.d \
./modbus_master_slave/functions/mbfuncdisc.d \
./modbus_master_slave/functions/mbfuncdisc_m.d \
./modbus_master_slave/functions/mbfuncholding.d \
./modbus_master_slave/functions/mbfuncholding_m.d \
./modbus_master_slave/functions/mbfuncinput.d \
./modbus_master_slave/functions/mbfuncinput_m.d \
./modbus_master_slave/functions/mbfuncother.d \
./modbus_master_slave/functions/mbutils.d 


# Each subdirectory must supply rules for building sources it contributes
modbus_master_slave/functions/%.o: ../modbus_master_slave/functions/%.c modbus_master_slave/functions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/MODBUS/modbus_workspaceNEW/f7RTU_freeMB_master_slave/modbus_master_slave/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

