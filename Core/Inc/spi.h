/**
 ******************************************************************************
 * @file           : spi.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : HAL-based SPI driver
 ******************************************************************************
*/
#ifndef SPI_H
#define SPI_H

#include "stm32f4xx_hal.h"
#include "stm32f411xe.h"
#include <stdint.h>

// Function prototypes
void SPI1_Init(void);
void SPI1_WriteByte(uint8_t data);

#endif
