################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/st7735/fonts.c \
../BSP/st7735/st7735.c 

OBJS += \
./BSP/st7735/fonts.o \
./BSP/st7735/st7735.o 

C_DEPS += \
./BSP/st7735/fonts.d \
./BSP/st7735/st7735.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/st7735/%.o BSP/st7735/%.su BSP/st7735/%.cyclo: ../BSP/st7735/%.c BSP/st7735/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-BSP-2f-st7735

clean-BSP-2f-st7735:
	-$(RM) ./BSP/st7735/fonts.cyclo ./BSP/st7735/fonts.d ./BSP/st7735/fonts.o ./BSP/st7735/fonts.su ./BSP/st7735/st7735.cyclo ./BSP/st7735/st7735.d ./BSP/st7735/st7735.o ./BSP/st7735/st7735.su

.PHONY: clean-BSP-2f-st7735

