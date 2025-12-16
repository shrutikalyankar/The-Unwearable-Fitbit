/**
 ******************************************************************************
 * @file           : heartrate.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : MAX30102(heartrate sensor, pulse oximeter, temperature
 * 					 reading) initialization, processing data
 * 					 from the sensor
 ******************************************************************************
*/

#include "heartrate.h"
#include "stm32f4xx_hal.h"
#include <math.h>
#include "uart.h"
#include "accelerometer.h"
#include "i2c.h"

#define BUFFER_SIZE 400 // 100hz
#define SAMPLE_WINDOW 10  // Number of samples to analyze for trend

static uint32_t red_buffer[BUFFER_SIZE];
static uint32_t ir_buffer[BUFFER_SIZE];
static uint8_t buffer_index = 0;

// Peak detector variables
static PeakDetectorState peak_state = STATE_WAITING;
static uint32_t sample_window[SAMPLE_WINDOW];

// init MAX30102
bool MAX30102_Init(void)
{
	 // check if can talk to it
	 if (checkDevice_hr()) {
	 	sendString("ERROR. Can't find hr device\n\r");
		  // return false;
	 }

	 // check part ID
	 uint8_t part_id = read_reg_hr(MAX30102_PART_ID);
	 if (part_id != 0x15) {  // MAX30102 Part ID
	 	sendString("ERROR. wrong part ID\n\r");
		  return false;
	 }

	 sendStringGreen("got it up and running!\n\r");

	 write_reg_hr(MAX30102_MODE_CONFIG, 0x40); // Software reset
	 HAL_Delay(50);

	 // averaging = 4, rollover enabled
	 write_reg_hr(MAX30102_FIFO_CONFIG, 0x4F);

	 write_reg_hr(MAX30102_MODE_CONFIG, MAX30102_MODE_SPO2); // SpO2 mode
	 write_reg_hr(MAX30102_SPO2_CONFIG, 0x27); //SP02: 100 Hz, 411us pulse width, 18-bit ADC

	 write_reg_hr(MAX30102_LED1_PA, 0x1F); // Red LED current
	 write_reg_hr(MAX30102_LED2_PA, 0x1F); // IR LED current

	 // clear the fifo
	 write_reg_hr(MAX30102_FIFO_WR_PTR, 0);
	 write_reg_hr(MAX30102_FIFO_OVF_CNT, 0);
	 write_reg_hr(MAX30102_FIFO_RD_PTR, 0);

	 // init buffer
	 buffer_index = 0;
	 for (int i = 0; i < BUFFER_SIZE; i++) {
		  red_buffer[i] = 0;
		  ir_buffer[i] = 0;
	 }
	 for (int i = 0; i < SAMPLE_WINDOW; i++) {
		 sample_window[i] = 0;
	 }


	 HAL_Delay(10);
	 return true;
}

// read in temp in C
float MAX30102_ReadTemperature(void)
{
	// put in temp mode
	write_reg_hr(MAX30102_TEMP_CONFIG, 0x01);
	HAL_Delay(100);

	int8_t temp_int = (int8_t)read_reg_hr(MAX30102_TEMP_INT);
	uint8_t temp_frac = read_reg_hr(MAX30102_TEMP_FRAC);
	return (float)temp_int + ((float)temp_frac * 0.0625f);
}

// get one sample from FIFO
void MAX30102_ReadFIFO(MAX30102_Sample *sample)
{
	uint8_t buf[6];
	// read in 6 bytes (3 red + 3 IR)
	read_regs_hr(MAX30102_FIFO_DATA, buf, 6);
	// convert to 18-bit values
	sample->red = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
	sample->red &= 0x03FFFF;
	sample->ir = ((uint32_t)buf[3] << 16) | ((uint32_t)buf[4] << 8) | buf[5];
	sample->ir &= 0x03FFFF;

	// store in my circular buffer IF valid
	if (sample->ir > 50000){
		red_buffer[buffer_index] = sample->red;
		ir_buffer[buffer_index] = sample->ir;
		buffer_index = (buffer_index + 1) % BUFFER_SIZE;
	}
}

// return if signal is increasing or decreasing based on window
static bool IsIncreasing(void)
{
    // comparing first half vs second half of window
    uint32_t first_half = 0;
    uint32_t second_half = 0;

    for (int i = 0; i < SAMPLE_WINDOW / 2; i++) {
        first_half += sample_window[i];
    }

    for (int i = SAMPLE_WINDOW / 2; i < SAMPLE_WINDOW; i++) {
        second_half += sample_window[i];
    }

    // increasing
    return (second_half > first_half);
}

// calc hr with peak detection
static bool CalculateHeartRate(uint8_t *heart_rate)
{
    // min and max for thresholding
    uint32_t min_val = ir_buffer[0];
    uint32_t max_val = ir_buffer[0];
    for (int i = 0; i < BUFFER_SIZE; i++) {
        if (ir_buffer[i] < min_val) min_val = ir_buffer[i];
        if (ir_buffer[i] > max_val) max_val = ir_buffer[i];
    }

    // make sure the finger is on the thing
    uint32_t range = max_val - min_val;
    if (range < 1000) {
        return false;
    }

    // count peaks using state machine
    uint32_t peaks = 0;
    peak_state = STATE_WAITING;

    // Process buffer in chunks of SAMPLE_WINDOW
    for (int i = 0; i < BUFFER_SIZE - SAMPLE_WINDOW; i += SAMPLE_WINDOW) {
        // Fill window with next 10 samples
        for (int j = 0; j < SAMPLE_WINDOW; j++) {
            sample_window[j] = ir_buffer[i + j];
        }

        bool increasing = IsIncreasing();
        PeakDetectorState last_state = peak_state;

        // State machine
        switch (peak_state) {
            case STATE_WAITING:
                if (increasing) {
                    peak_state = STATE_INCREASING;
                }
                break;

            case STATE_INCREASING:
                if (!increasing) {
                    // inc -> dec transition = peak
                    peaks++;
                    peak_state = STATE_DECREASING;
                }
                break;

            case STATE_DECREASING:
                if (increasing) {
                    peak_state = STATE_INCREASING;
                }
                break;
        }
    }

    // calc bpm (recorded for BUFFER_SIZE/100 seconds, need per min)
    *heart_rate = peaks * 60 / (BUFFER_SIZE / 100) / 1.6f;

    if (*heart_rate < 40 || *heart_rate > 200) {
        // return false; // wayy out of range
    }
    return true;
}


// get SpO2 w AC/DC ratio method
static bool CalculateSpO2(uint8_t *spo2)
{
	// DC (average) values
	uint32_t red_dc = 0;
	uint32_t ir_dc = 0;
	for (int i = 0; i < BUFFER_SIZE; i++) {
		red_dc += red_buffer[i];
		ir_dc += ir_buffer[i];
	}
	red_dc /= BUFFER_SIZE;
	ir_dc /= BUFFER_SIZE;

	// AC values are just pk to pk
	uint32_t red_min = red_buffer[0];
	uint32_t red_max = red_buffer[0];
	uint32_t ir_min = ir_buffer[0];
	uint32_t ir_max = ir_buffer[0];
	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (red_buffer[i] < red_min) red_min = red_buffer[i];
		if (red_buffer[i] > red_max) red_max = red_buffer[i];
		if (ir_buffer[i] < ir_min) ir_min = ir_buffer[i];
		if (ir_buffer[i] > ir_max) ir_max = ir_buffer[i];
	}
	uint32_t red_ac = red_max - red_min;
	uint32_t ir_ac = ir_max - ir_min;

	if (red_ac < 100 || ir_ac < 100) {
		return false;  // finger isn't on the sensor
	}

	// R = (AC_red * DC_ir) / (DC_red * AC_ir)
	if (red_dc == 0 || ir_ac == 0) {
		return false; // can't div by 0
	}
	float R = ((float)red_ac * (float)ir_dc) / ((float)red_dc * (float)ir_ac);

	float spo2_value = -45.060 * R * R + 30.354 * R + 109.845; // edited other calibration curve
	// float spo2_value = 110.0f - 25.0f * R; // not my calibration curve
	if (spo2_value > 100.0f) spo2_value = 100.0f;
	// if (spo2_value < 80.0f) return false;  // omg I hope it's not below 80% haha
	*spo2 = (uint8_t)spo2_value;
	return true;
}

// update all measurements
bool MAX30102_Update(MAX30102_Data *data)
{
	MAX30102_Sample sample;
	MAX30102_ReadFIFO(&sample);

	// Check for finger
	if (sample.ir < 50000) {
		// sendString("No fingerrr\n\r");
		data->valid = false;
		return false;
	}

	// Read temp
	data->temperature = MAX30102_ReadTemperature();

	// heart rate
	if (!CalculateHeartRate(&data->heart_rate)) {
		data->valid = false;
		return false;
	}

	// SpO2
	if (!CalculateSpO2(&data->spo2)) {
		data->valid = false;
		return false;
	}

	data->valid = true;
	return true;
}

