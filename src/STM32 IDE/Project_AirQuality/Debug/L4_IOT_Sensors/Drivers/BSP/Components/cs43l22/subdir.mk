################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.c 

OBJS += \
./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.o 

C_DEPS += \
./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.d 


# Each subdirectory must supply rules for building sources it contributes
L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/%.o L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/%.su L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/%.cyclo: ../L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/%.c L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-Components-2f-cs43l22

clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-Components-2f-cs43l22:
	-$(RM) ./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.cyclo ./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.d ./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.o ./L4_IOT_Sensors/Drivers/BSP/Components/cs43l22/cs43l22.su

.PHONY: clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-Components-2f-cs43l22

