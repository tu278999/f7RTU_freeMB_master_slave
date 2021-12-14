################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../modbus_master_slave/port/e_port.c \
../modbus_master_slave/port/portevent.c \
../modbus_master_slave/port/portevent_m.c \
../modbus_master_slave/port/portserial.c \
../modbus_master_slave/port/portserial_m.c \
../modbus_master_slave/port/porttimer.c \
../modbus_master_slave/port/porttimer_m.c \
../modbus_master_slave/port/user_mb_app.c \
../modbus_master_slave/port/user_mb_app_m.c 

OBJS += \
./modbus_master_slave/port/e_port.o \
./modbus_master_slave/port/portevent.o \
./modbus_master_slave/port/portevent_m.o \
./modbus_master_slave/port/portserial.o \
./modbus_master_slave/port/portserial_m.o \
./modbus_master_slave/port/porttimer.o \
./modbus_master_slave/port/porttimer_m.o \
./modbus_master_slave/port/user_mb_app.o \
./modbus_master_slave/port/user_mb_app_m.o 

C_DEPS += \
./modbus_master_slave/port/e_port.d \
./modbus_master_slave/port/portevent.d \
./modbus_master_slave/port/portevent_m.d \
./modbus_master_slave/port/portserial.d \
./modbus_master_slave/port/portserial_m.d \
./modbus_master_slave/port/porttimer.d \
./modbus_master_slave/port/porttimer_m.d \
./modbus_master_slave/port/user_mb_app.d \
./modbus_master_slave/port/user_mb_app_m.d 


# Each subdirectory must supply rules for building sources it contributes
modbus_master_slave/port/%.o: ../modbus_master_slave/port/%.c modbus_master_slave/port/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F767xx -c -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -I"E:/OneDrive - Hanoi University of Science and Technology/Document_TuBK/MODBUS/modbus_workspaceNEW/f7RTU_freeMB_master_slave/modbus_master_slave/include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

