################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus_master_slave/rtu/mbcrc.c \
../modbus_master_slave/rtu/mbrtu.c \
../modbus_master_slave/rtu/mbrtu_m.c 

OBJS += \
./modbus_master_slave/rtu/mbcrc.o \
./modbus_master_slave/rtu/mbrtu.o \
./modbus_master_slave/rtu/mbrtu_m.o 

C_DEPS += \
./modbus_master_slave/rtu/mbcrc.d \
./modbus_master_slave/rtu/mbrtu.d \
./modbus_master_slave/rtu/mbrtu_m.d 


# Each subdirectory must supply rules for building sources it contributes
modbus_master_slave/rtu/%.o: ../modbus_master_slave/rtu/%.c modbus_master_slave/rtu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/MODBUS/modbus_workspaceNEW/f7RTU_freeMB_master_slave/modbus_master_slave/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
