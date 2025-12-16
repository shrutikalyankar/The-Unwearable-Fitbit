/**
 ******************************************************************************
 * @file           : heartrate.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : MAX30102(heartrate sensor, pulse oximeter, temperature
 * 					 reading) initialization, processing data
 * 					 from the sensor
 ******************************************************************************
*/

#ifndef HEARTRATE_H
#define HEARTRATE_H

#include <stdint.h>
#include <stdbool.h>

//reg addresses
#define MAX30102_INT_STATUS_1    0x00
#define MAX30102_FIFO_WR_PTR     0x04
#define MAX30102_FIFO_OVF_CNT    0x05
#define MAX30102_FIFO_RD_PTR     0x06
#define MAX30102_FIFO_DATA       0x07
#define MAX30102_FIFO_CONFIG     0x08
#define MAX30102_MODE_CONFIG     0x09
#define MAX30102_SPO2_CONFIG     0x0A
#define MAX30102_LED1_PA         0x0C  // red LED
#define MAX30102_LED2_PA         0x0D  // IR LED
#define MAX30102_TEMP_INT        0x1F
#define MAX30102_TEMP_FRAC       0x20
#define MAX30102_TEMP_CONFIG     0x21
#define MAX30102_PART_ID         0xFF

// mode config
#define MAX30102_MODE_SPO2       0x03

typedef enum {
    STATE_WAITING,
	STATE_INCREASING,
	STATE_DECREASING
} PeakDetectorState;

typedef struct {
    uint32_t red;
    uint32_t ir;
} MAX30102_Sample;

typedef struct {
    uint8_t heart_rate;
    uint8_t spo2;
    float temperature;
    bool valid; // if there's no finger on it, wont say data is valid
} MAX30102_Data;

bool MAX30102_Init(void);
void MAX30102_ReadFIFO(MAX30102_Sample *sample);
float MAX30102_ReadTemperature(void);
bool MAX30102_Update(MAX30102_Data *data); // main update func

#endif
