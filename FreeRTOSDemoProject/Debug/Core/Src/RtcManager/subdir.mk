################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/RtcManager/RtcManager.c 

OBJS += \
./Core/Src/RtcManager/RtcManager.o 

C_DEPS += \
./Core/Src/RtcManager/RtcManager.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/RtcManager/%.o Core/Src/RtcManager/%.su Core/Src/RtcManager/%.cyclo: ../Core/Src/RtcManager/%.c Core/Src/RtcManager/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/AccManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/RtcManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/LedManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/Core/Src/UartManager" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F" -I"/home/chris/Documents/portfolio/stm32f407-freertos-demo/FreeRTOSDemoProject/ThirdParty/FreeRTOS/include" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-RtcManager

clean-Core-2f-Src-2f-RtcManager:
	-$(RM) ./Core/Src/RtcManager/RtcManager.cyclo ./Core/Src/RtcManager/RtcManager.d ./Core/Src/RtcManager/RtcManager.o ./Core/Src/RtcManager/RtcManager.su

.PHONY: clean-Core-2f-Src-2f-RtcManager

