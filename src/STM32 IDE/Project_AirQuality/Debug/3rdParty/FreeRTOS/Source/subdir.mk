################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../3rdParty/FreeRTOS/Source/croutine.c \
../3rdParty/FreeRTOS/Source/event_groups.c \
../3rdParty/FreeRTOS/Source/list.c \
../3rdParty/FreeRTOS/Source/queue.c \
../3rdParty/FreeRTOS/Source/stream_buffer.c \
../3rdParty/FreeRTOS/Source/tasks.c \
../3rdParty/FreeRTOS/Source/timers.c 

OBJS += \
./3rdParty/FreeRTOS/Source/croutine.o \
./3rdParty/FreeRTOS/Source/event_groups.o \
./3rdParty/FreeRTOS/Source/list.o \
./3rdParty/FreeRTOS/Source/queue.o \
./3rdParty/FreeRTOS/Source/stream_buffer.o \
./3rdParty/FreeRTOS/Source/tasks.o \
./3rdParty/FreeRTOS/Source/timers.o 

C_DEPS += \
./3rdParty/FreeRTOS/Source/croutine.d \
./3rdParty/FreeRTOS/Source/event_groups.d \
./3rdParty/FreeRTOS/Source/list.d \
./3rdParty/FreeRTOS/Source/queue.d \
./3rdParty/FreeRTOS/Source/stream_buffer.d \
./3rdParty/FreeRTOS/Source/tasks.d \
./3rdParty/FreeRTOS/Source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
3rdParty/FreeRTOS/Source/%.o 3rdParty/FreeRTOS/Source/%.su 3rdParty/FreeRTOS/Source/%.cyclo: ../3rdParty/FreeRTOS/Source/%.c 3rdParty/FreeRTOS/Source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L4S5xx -c -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/lps22hb" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Src/Drivers" -I"C:/Interfacing/workspace/Project_AirQuality/Core/Inc/Drivers" -I../Core/Inc -I"C:/Interfacing/workspace/Project_AirQuality/BSP/st7735" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/Components/hts221" -I"C:/Interfacing/workspace/Project_AirQuality/L4_IOT_Sensors/Drivers/BSP/B-L475E-IOT01" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Interfacing/workspace/Project_AirQuality/3rdParty/FreeRTOS/Source/include" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-3rdParty-2f-FreeRTOS-2f-Source

clean-3rdParty-2f-FreeRTOS-2f-Source:
	-$(RM) ./3rdParty/FreeRTOS/Source/croutine.cyclo ./3rdParty/FreeRTOS/Source/croutine.d ./3rdParty/FreeRTOS/Source/croutine.o ./3rdParty/FreeRTOS/Source/croutine.su ./3rdParty/FreeRTOS/Source/event_groups.cyclo ./3rdParty/FreeRTOS/Source/event_groups.d ./3rdParty/FreeRTOS/Source/event_groups.o ./3rdParty/FreeRTOS/Source/event_groups.su ./3rdParty/FreeRTOS/Source/list.cyclo ./3rdParty/FreeRTOS/Source/list.d ./3rdParty/FreeRTOS/Source/list.o ./3rdParty/FreeRTOS/Source/list.su ./3rdParty/FreeRTOS/Source/queue.cyclo ./3rdParty/FreeRTOS/Source/queue.d ./3rdParty/FreeRTOS/Source/queue.o ./3rdParty/FreeRTOS/Source/queue.su ./3rdParty/FreeRTOS/Source/stream_buffer.cyclo ./3rdParty/FreeRTOS/Source/stream_buffer.d ./3rdParty/FreeRTOS/Source/stream_buffer.o ./3rdParty/FreeRTOS/Source/stream_buffer.su ./3rdParty/FreeRTOS/Source/tasks.cyclo ./3rdParty/FreeRTOS/Source/tasks.d ./3rdParty/FreeRTOS/Source/tasks.o ./3rdParty/FreeRTOS/Source/tasks.su ./3rdParty/FreeRTOS/Source/timers.cyclo ./3rdParty/FreeRTOS/Source/timers.d ./3rdParty/FreeRTOS/Source/timers.o ./3rdParty/FreeRTOS/Source/timers.su

.PHONY: clean-3rdParty-2f-FreeRTOS-2f-Source

