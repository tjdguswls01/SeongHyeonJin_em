################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Driver/uart_com/uart_com.c 

OBJS += \
./Core/Driver/uart_com/uart_com.o 

C_DEPS += \
./Core/Driver/uart_com/uart_com.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Driver/uart_com/%.o Core/Driver/uart_com/%.su Core/Driver/uart_com/%.cyclo: ../Core/Driver/uart_com/%.c Core/Driver/uart_com/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Driver-2f-uart_com

clean-Core-2f-Driver-2f-uart_com:
	-$(RM) ./Core/Driver/uart_com/uart_com.cyclo ./Core/Driver/uart_com/uart_com.d ./Core/Driver/uart_com/uart_com.o ./Core/Driver/uart_com/uart_com.su

.PHONY: clean-Core-2f-Driver-2f-uart_com

