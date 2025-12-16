/**
 ******************************************************************************
 * @file           : accelerometer.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : LSM303DLHC (the on board accelerometer) driver for STM32
 * 					 setting up with 100Hz, ±8g, high-res
 ******************************************************************************
*/

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include <stdbool.h>

// will have the x, y, z data in terms of g's
typedef struct {
    float x;
    float y;
    float z;
} AccelRawData;

bool Accel_Init(void);
void Accel_ReadRaw(AccelRawData *data);

#endif
