/**
 ******************************************************************************
 * @file           : i2c.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Shared I2C driver for accelerometer and heartrate sensor.
 * 				     Getting data from both the sensors
 ******************************************************************************
*/

#include "i2c.h"
#include "accelerometer.h"

I2C_HandleTypeDef hi2c1;

void init_i2c(void) {
    __HAL_RCC_I2C1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // PB6=SCL, PB9=SDA
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = GPIO_PIN_6 | GPIO_PIN_9;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_PULLUP;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &gpio);

    // I2C1, 400kHz
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    HAL_I2C_Init(&hi2c1); // built in func they give you
}

/* accelerometer*/

void write_reg_accel(uint8_t reg, uint8_t val) {
    HAL_I2C_Mem_Write(&hi2c1, LSM303_ADDR, reg,
                      I2C_MEMADD_SIZE_8BIT, &val, 1, I2C_TIMEOUT);
}

void read_regs_accel(uint8_t reg, uint8_t *buf, uint8_t len) {
    reg |= 0x80;  // auto increment
    HAL_I2C_Mem_Read(&hi2c1, LSM303_ADDR, reg,
                     I2C_MEMADD_SIZE_8BIT, buf, len, I2C_TIMEOUT);
}

bool checkDevice_accel(){
	return (HAL_I2C_IsDeviceReady(&hi2c1, LSM303_ADDR, 3, I2C_TIMEOUT) != HAL_OK);
}

/* hr sensor*/
void write_reg_hr(uint8_t reg, uint8_t val) {
    HAL_I2C_Mem_Write(&hi2c1, MAX30102_I2C_ADDR, reg,
                      I2C_MEMADD_SIZE_8BIT, &val, 1, I2C_TIMEOUT);
}

uint8_t read_reg_hr(uint8_t reg) {
    uint8_t val;
    HAL_I2C_Mem_Read(&hi2c1, MAX30102_I2C_ADDR, reg,
                     I2C_MEMADD_SIZE_8BIT, &val, 1, I2C_TIMEOUT);
    return val;
}

void read_regs_hr(uint8_t reg, uint8_t *buf, uint8_t len) {
    HAL_I2C_Mem_Read(&hi2c1, MAX30102_I2C_ADDR, reg,
                     I2C_MEMADD_SIZE_8BIT, buf, len, I2C_TIMEOUT);
}

bool checkDevice_hr(){
	return (HAL_I2C_IsDeviceReady(&hi2c1, MAX30102_I2C_ADDR, 3, I2C_TIMEOUT) != HAL_OK);
}
