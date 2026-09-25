################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Drivers/PMS5003.c \
../Core/Src/Drivers/PMS5003_HAL_STM32.c \
../Core/Src/Drivers/driver_sgp30.c \
../Core/Src/Drivers/sensirion_common.c \
../Core/Src/Drivers/sensirion_i2c.c 

OBJS += \
./Core/Src/Drivers/PMS5003.o \
./Core/Src/Drivers/PMS5003_HAL_STM32.o \
./Core/Src/Drivers/driver_sgp30.o \
./Core/Src/Drivers/sensirion_common.o \
./Core/Src/Drivers/sensirion_i2c.o 

C_DEPS += \
./Core/Src/Drivers/PMS5003.d \
./Core/Src/Drivers/PMS5003_HAL_STM32.d \
./Core/Src/Drivers/driver_sgp30.d \
./Core/Src/Drivers/sensirion_common.d \
./Core/Src/Drivers/sensirion_i2c.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Drivers/%.o Core/Src/Drivers/%.su Core/Src/Drivers/%.cyclo: ../Core/Src/Drivers/%.c Core/Src/Drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Drivers

clean-Core-2f-Src-2f-Drivers:
	-$(RM) ./Core/Src/Drivers/PMS5003.cyclo ./Core/Src/Drivers/PMS5003.d ./Core/Src/Drivers/PMS5003.o ./Core/Src/Drivers/PMS5003.su ./Core/Src/Drivers/PMS5003_HAL_STM32.cyclo ./Core/Src/Drivers/PMS5003_HAL_STM32.d ./Core/Src/Drivers/PMS5003_HAL_STM32.o ./Core/Src/Drivers/PMS5003_HAL_STM32.su ./Core/Src/Drivers/driver_sgp30.cyclo ./Core/Src/Drivers/driver_sgp30.d ./Core/Src/Drivers/driver_sgp30.o ./Core/Src/Drivers/driver_sgp30.su ./Core/Src/Drivers/sensirion_common.cyclo ./Core/Src/Drivers/sensirion_common.d ./Core/Src/Drivers/sensirion_common.o ./Core/Src/Drivers/sensirion_common.su ./Core/Src/Drivers/sensirion_i2c.cyclo ./Core/Src/Drivers/sensirion_i2c.d ./Core/Src/Drivers/sensirion_i2c.o ./Core/Src/Drivers/sensirion_i2c.su

.PHONY: clean-Core-2f-Src-2f-Drivers

