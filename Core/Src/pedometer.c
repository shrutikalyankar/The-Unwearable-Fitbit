/**
 ******************************************************************************
 * @file           : pedometer.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Step counter, calculate BMR, calories
 ******************************************************************************
*/

#include "pedometer.h"
#include "uart.h"

// Thresholds for step detection
#define STEP_THRESHOLD_LOW   -0.05f // has to go below this
#define STEP_THRESHOLD_HIGH   0.05f // and then above this to count as a step

// state vars for pedometer
static uint32_t step_count = 0;
static float stride_length = 0.75f; // to calc dist traveled (in m)
static bool below_threshold = false;

// user profile for bmr cals
static UserProfile user = {125.0f, 64.0f, 22, false};
float bmr_per_sec;

// using Mifflin-St Jeor Equation
// converts to kg/cm internally
void CalculateBMR(void)
{
    float weight_kg = user.weight_lbs * 0.453592f;
    float height_cm = user.height_inches * 2.54f;

    float bmr_per_day;

    if (user.is_male) {
        // men: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) + 5
        bmr_per_day = (10.0f * weight_kg) + (6.25f * height_cm) - (5.0f * user.age) + 5.0f;
    } else {
        // women: BMR = (10 × weight in kg) + (6.25 × height in cm) - (5 × age in years) - 161
        bmr_per_day = (10.0f * weight_kg) + (6.25f * height_cm) - (5.0f * user.age) - 161.0f;
    }
    // convert to cal/s
    bmr_per_sec = bmr_per_day / 86400.0f;
}

// update steps based on accelerometer reading
void Pedometer_Update(AccelRawData *accel_data)
{
    float y = accel_data->y;

    if (y < STEP_THRESHOLD_LOW) {
        below_threshold = true;
    }

    // if went above threshold after being below
    if (below_threshold && y > STEP_THRESHOLD_HIGH){

    	// debounce
        static uint32_t debounce_counter = 0;
        if (debounce_counter == 0) {
            step_count++;
            debounce_counter = 20;  // this is like 200 ms
            below_threshold = false;
        }

        if (debounce_counter > 0) {
            debounce_counter--;
        }
    }
}

//store all values in struct variables
void Pedometer_GetStats(PedometerData *data)
{
    data->steps = step_count;
    data->distance = (float)step_count * stride_length; // steps * length
    data->bmr_cals += bmr_per_sec; // called every s
    data->calories = step_count * 0.04f + data->bmr_cals; // around 0.04 cal per step
}
