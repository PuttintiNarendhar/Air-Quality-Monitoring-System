################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.c \
../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.c 

OBJS += \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.o \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.o 

C_DEPS += \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.d \
./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.d 


# Each subdirectory must supply rules for building sources it contributes
L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/%.o L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/%.su L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/%.cyclo: ../L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/%.c L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-B-2d-L475E-2d-IOT01

clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-B-2d-L475E-2d-IOT01:
	-$(RM) ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_accelero.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_gyro.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_hsensor.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_magneto.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_psensor.su ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.cyclo ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.d ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.o ./L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01/stm32l475e_iot01_tsensor.su

.PHONY: clean-L4_IOT_Sensors-2f-Drivers-2f-BSP-2f-B-2d-L475E-2d-IOT01

