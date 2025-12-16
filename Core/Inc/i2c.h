/**
 ******************************************************************************
 * @file           : i2c.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Shared I2C driver for accelerometer and heartrate sensor.
 * 				     Getting data from both the sensors
 ******************************************************************************
*/

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define I2C_TIMEOUT             100
#define LSM303_ADDR             (0x19 << 1)  // 0x32 after shift
#define MAX30102_I2C_ADDR   (0x57 << 1)  // 0xAE after shift

void init_i2c(void);
// accelerometer
void write_reg_accel(uint8_t reg, uint8_t val);
void read_regs_accel(uint8_t reg, uint8_t *buf, uint8_t len);
bool checkDevice_accel();
// hr sensor
void write_reg_hr(uint8_t reg, uint8_t val);
uint8_t read_reg_hr(uint8_t reg);
void read_regs_hr(uint8_t reg, uint8_t *buf, uint8_t len);
bool checkDevice_hr();
