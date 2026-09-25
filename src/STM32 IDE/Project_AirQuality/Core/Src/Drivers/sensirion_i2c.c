#include "sensirion_i2c.h"
#include "stm32l4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;   // <-- MUST match your CubeMX I2C

int8_t sensirion_i2c_read(uint8_t address, uint8_t* data, uint16_t count)
{
    if (HAL_I2C_Master_Receive(&hi2c1,
                               address,   // <-- NO shift here
                               data,
                               count,
                               100) == HAL_OK)
    {
        return 0;
    }
    return -1;
}

int8_t sensirion_i2c_write(uint8_t address, const uint8_t* data, uint16_t count)
{
    if (HAL_I2C_Master_Transmit(&hi2c1,
                                address,   // <-- NO shift here
                                (uint8_t*)data,
                                count,
                                100) == HAL_OK)
    {
        return 0;
    }
    return -1;
}


void sensirion_sleep_usec(uint32_t useconds)
{
    // microsecond precision not required → ms is fine
    HAL_Delay((useconds + 999) / 1000);
}
