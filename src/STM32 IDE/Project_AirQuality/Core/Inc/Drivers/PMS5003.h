/*
 * PMS5003.h
 *
 *  Created on: Dec 10, 2025
 *      Author: Khalid
 */

#ifndef PMS5003_H
#define PMS5003_H

#include <stdint.h>
#include <stddef.h>

/**
 * Data structure holding the latest PMS5003 reading.
 */
typedef struct {
    uint16_t PM1_0;
    uint16_t PM2_5;
    uint16_t PM10;
} PMS5003_Data_TypeDef;

/**
 * Initialize driver: start UART reception for PMS5003.
 * Call this after HAL UART is initialized.
 */
void PMS5003_StartReception(void);

/**
 * Process received bytes — must be called from HAL_UART_RxCpltCallback
 * when a new byte arrives.
 *
 * Return 1 if a new valid packet was received and parsed,
 * otherwise 0.
 */
int PMS5003_ParseData(uint8_t rx_byte);

/**
 * Get the most recent valid data.
 */
PMS5003_Data_TypeDef PMS5003_GetLatestData(void);

#endif /* PMS5003_H */

