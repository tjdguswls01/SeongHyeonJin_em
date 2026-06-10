################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Driver/ir_sensor/ir_sensor.c 

OBJS += \
./Core/Driver/ir_sensor/ir_sensor.o 

C_DEPS += \
./Core/Driver/ir_sensor/ir_sensor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Driver/ir_sensor/%.o Core/Driver/ir_sensor/%.su Core/Driver/ir_sensor/%.cyclo: ../Core/Driver/ir_sensor/%.c Core/Driver/ir_sensor/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Driver-2f-ir_sensor

clean-Core-2f-Driver-2f-ir_sensor:
	-$(RM) ./Core/Driver/ir_sensor/ir_sensor.cyclo ./Core/Driver/ir_sensor/ir_sensor.d ./Core/Driver/ir_sensor/ir_sensor.o ./Core/Driver/ir_sensor/ir_sensor.su

.PHONY: clean-Core-2f-Driver-2f-ir_sensor

