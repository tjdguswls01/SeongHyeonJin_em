################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Driver/gpio_signal/gpio_signal.c 

OBJS += \
./Core/Driver/gpio_signal/gpio_signal.o 

C_DEPS += \
./Core/Driver/gpio_signal/gpio_signal.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Driver/gpio_signal/%.o Core/Driver/gpio_signal/%.su Core/Driver/gpio_signal/%.cyclo: ../Core/Driver/gpio_signal/%.c Core/Driver/gpio_signal/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Driver-2f-gpio_signal

clean-Core-2f-Driver-2f-gpio_signal:
	-$(RM) ./Core/Driver/gpio_signal/gpio_signal.cyclo ./Core/Driver/gpio_signal/gpio_signal.d ./Core/Driver/gpio_signal/gpio_signal.o ./Core/Driver/gpio_signal/gpio_signal.su

.PHONY: clean-Core-2f-Driver-2f-gpio_signal

