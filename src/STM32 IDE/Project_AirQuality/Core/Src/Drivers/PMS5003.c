/*
 * PMS5003.c
 *
 *  Created on: Dec 10, 2025
 *      Author: Khalid
 */

#include "PMS5003.h"
//#include "stm32f4xx_hal.h"   // or correct HAL header for your STM32 series
#include "stm32l4xx_hal.h"
#include <string.h>

/**
 * Configuration: adjust this to your UART handle.
 * E.g. extern UART_HandleTypeDef huart1;
 */
extern UART_HandleTypeDef PMS5003_UART_HANDLE;

/* PMS5003 protocol constants */
#define PMS_HEADER_HIGH   0x42
#define PMS_HEADER_LOW    0x4D
#define PMS_FRAME_LENGTH  32  // bytes total for PMS5003 standard frame

/* static buffer and state for assembly & parsing */
static uint8_t pms_rx_buf[PMS_FRAME_LENGTH];
static size_t  pms_rx_index = 0;

/* Latest parsed data */
static PMS5003_Data_TypeDef latest_data = { 0, 0, 0 };

/* Forward declaration for HAL callback bridging */
void PMS5003_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * Start the UART in interrupt mode to receive 1 byte at a time.
 */
void PMS5003_StartReception(void) {
    /* start receiving a byte via interrupt */
    HAL_UART_Receive_IT(&PMS5003_UART_HANDLE, (uint8_t *)&pms_rx_buf[pms_rx_index], 1);
}

/**
 * Call this from HAL_UART_RxCpltCallback when a byte is received.
 */
int PMS5003_ParseData(uint8_t rx_byte) {
    pms_rx_buf[pms_rx_index++] = rx_byte;

    if (pms_rx_index == 1) {
        if (rx_byte != PMS_HEADER_HIGH) {
            /* bad start — restart */
            pms_rx_index = 0;
            return 0;
        }
    } else if (pms_rx_index == 2) {
        if (rx_byte != PMS_HEADER_LOW) {
            /* invalid header — shift buffer */
            pms_rx_buf[0] = rx_byte;
            pms_rx_index = 1;
            return 0;
        }
    }

    if (pms_rx_index >= PMS_FRAME_LENGTH) {
        /* full frame received — parse */

        /* verify checksum */
        uint16_t frame_len = (pms_rx_buf[2] << 8) | pms_rx_buf[3];
        if (frame_len + 4 > PMS_FRAME_LENGTH) {
            /* invalid length */
            pms_rx_index = 0;
            return 0;
        }

        uint16_t sum = 0;
        for (size_t i = 0; i < PMS_FRAME_LENGTH - 2; i++) {
            sum += pms_rx_buf[i];
        }
        uint16_t checksum = (pms_rx_buf[PMS_FRAME_LENGTH - 2] << 8) |
                             pms_rx_buf[PMS_FRAME_LENGTH - 1];
        if (sum != checksum) {
            /* bad checksum */
            pms_rx_index = 0;
            return 0;
        }

        /* parse data: PM1.0, PM2.5, PM10 (standard particles) */
        latest_data.PM1_0 = (pms_rx_buf[10] << 8) | pms_rx_buf[11];
        latest_data.PM2_5 = (pms_rx_buf[12] << 8) | pms_rx_buf[13];
        latest_data.PM10  = (pms_rx_buf[14] << 8) | pms_rx_buf[15];

        pms_rx_index = 0;
        return 1;
    }

    /* continue receiving */
    HAL_UART_Receive_IT(&PMS5003_UART_HANDLE, &pms_rx_buf[pms_rx_index], 1);
    return 0;
}

PMS5003_Data_TypeDef PMS5003_GetLatestData(void) {
    return latest_data;
}

/**
 * Bridge function for HAL interrupt callback.
 * In your global HAL_UART_RxCpltCallback, call this:
 *
 * void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
 *     if (huart == &PMS5003_UART_HANDLE) {
 *         PMS5003_UART_RxCpltCallback(huart);
 *     }
 * }
 */
void PMS5003_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uint8_t byte = pms_rx_buf[pms_rx_index];
    if (PMS5003_ParseData(byte)) {
        // new data ready — optionally signal via flag/semaphore
    }
}

