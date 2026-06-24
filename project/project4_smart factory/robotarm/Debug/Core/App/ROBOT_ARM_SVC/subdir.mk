################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/App/ROBOT_ARM_SVC/robot_arm.c 

OBJS += \
./Core/App/ROBOT_ARM_SVC/robot_arm.o 

C_DEPS += \
./Core/App/ROBOT_ARM_SVC/robot_arm.d 


# Each subdirectory must supply rules for building sources it contributes
Core/App/ROBOT_ARM_SVC/%.o Core/App/ROBOT_ARM_SVC/%.su Core/App/ROBOT_ARM_SVC/%.cyclo: ../Core/App/ROBOT_ARM_SVC/%.c Core/App/ROBOT_ARM_SVC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-App-2f-ROBOT_ARM_SVC

clean-Core-2f-App-2f-ROBOT_ARM_SVC:
	-$(RM) ./Core/App/ROBOT_ARM_SVC/robot_arm.cyclo ./Core/App/ROBOT_ARM_SVC/robot_arm.d ./Core/App/ROBOT_ARM_SVC/robot_arm.o ./Core/App/ROBOT_ARM_SVC/robot_arm.su

.PHONY: clean-Core-2f-App-2f-ROBOT_ARM_SVC

