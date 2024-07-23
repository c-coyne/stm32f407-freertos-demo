################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/UartManager/UartManager.c 

OBJS += \
./Core/Src/UartManager/UartManager.o 

C_DEPS += \
./Core/Src/UartManager/UartManager.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/UartManager/%.o Core/Src/UartManager/%.su Core/Src/UartManager/%.cyclo: ../Core/Src/UartManager/%.c Core/Src/UartManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/MotorManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/SEGGER/Config" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/SEGGER/OS" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/SEGGER/SEGGER" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/AccManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/RtcManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/LedManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/UartManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-UartManager

clean-Core-2f-Src-2f-UartManager:
	-$(RM) ./Core/Src/UartManager/UartManager.cyclo ./Core/Src/UartManager/UartManager.d ./Core/Src/UartManager/UartManager.o ./Core/Src/UartManager/UartManager.su

.PHONY: clean-Core-2f-Src-2f-UartManager

