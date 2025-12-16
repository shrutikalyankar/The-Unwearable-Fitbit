/**
 ******************************************************************************
 * @file           : pedometer.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Step counter, calculate BMR, calories
 ******************************************************************************
*/

#ifndef PEDOMETER_H
#define PEDOMETER_H

#include <stdint.h>
#include <stdbool.h>
#include "accelerometer.h"

// pedometer data struct
typedef struct {
    uint32_t steps;
    float distance;
    float bmr_cals;
    float calories;
} PedometerData;

typedef struct {
    float weight_lbs;
    float height_inches;
	uint32_t age;
    bool is_male;
} UserProfile;

void Pedometer_Update(AccelRawData *accel_data);
void Pedometer_GetStats(PedometerData *data);
void CalculateBMR(void);

#endif

