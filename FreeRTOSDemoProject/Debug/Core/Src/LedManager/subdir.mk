################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/LedManager/LedManager.c 

OBJS += \
./Core/Src/LedManager/LedManager.o 

C_DEPS += \
./Core/Src/LedManager/LedManager.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/LedManager/%.o Core/Src/LedManager/%.su Core/Src/LedManager/%.cyclo: ../Core/Src/LedManager/%.c Core/Src/LedManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/AccManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/RtcManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/LedManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/UartManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-LedManager

clean-Core-2f-Src-2f-LedManager:
	-$(RM) ./Core/Src/LedManager/LedManager.cyclo ./Core/Src/LedManager/LedManager.d ./Core/Src/LedManager/LedManager.o ./Core/Src/LedManager/LedManager.su

.PHONY: clean-Core-2f-Src-2f-LedManager

