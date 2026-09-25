################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.o 3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.su 3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.cyclo: ../3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.c 3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-3rdParty-2f-FreeRTOS-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F

clean-3rdParty-2f-FreeRTOS-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F:
	-$(RM) ./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.cyclo ./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.d ./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.o ./3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.su

.PHONY: clean-3rdParty-2f-FreeRTOS-2f-Source-2f-portable-2f-GCC-2f-ARM_CM4F

