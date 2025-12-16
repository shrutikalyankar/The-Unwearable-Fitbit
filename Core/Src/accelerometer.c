/**
 ******************************************************************************
 * @file           : accelerometer.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : LSM303DLHC (the on board accelerometer) driver for STM32
 * 					 setting up with 100Hz, ±8g, high-res
 ******************************************************************************
*/

#include "accelerometer.h"
#include "stm32f4xx_hal.h"
#include "uart.h"
#include "i2c.h"

// registers
#define CTRL_REG1_A             0x20
#define CTRL_REG4_A             0x23
#define OUT_X_L_A               0x28

// again, setting to fixed config. These are the hard-coded values
#define CTRL1_100HZ_ENABLED     0x57 // 100Hz, all axes enabled
#define CTRL4_8G_HIGHRES_BDU    0xA8 // ±8g, high-res, BDU enabled

/*Checks we are connected to the accelerometer. Write using control registers*/
bool Accel_Init(void) {
    // make sure we are connected
    if(checkDevice_accel()){
    	sendString("ACCEL CONNECTION FAILED\n\r");
        return false;
    }

    // config
    write_reg_accel(CTRL_REG1_A, CTRL1_100HZ_ENABLED);
    write_reg_accel(CTRL_REG4_A, CTRL4_8G_HIGHRES_BDU);

    HAL_Delay(10);
    return true;
}

/*helper function called in Accel_ReadRaw*/
void convert_to_gs(AccelRawData *data){
	data->x = (data->x * 8.0f) / 2048.0f;
	data->y = (data->y * 8.0f) / 2048.0f;
	data->z = (data->z * 8.0f) / 2048.0f;
}

/*read all values*/
void Accel_ReadRaw(AccelRawData *data) {
    uint8_t buf[6];

    read_regs_accel(OUT_X_L_A, buf, 6); // X_L, X_H, Y_L, Y_H, Z_L, Z_H

    // Combine bytes
    data->x = (int16_t)((buf[1] << 8) | buf[0]) >> 4;
    data->y = (int16_t)((buf[3] << 8) | buf[2]) >> 4;
    data->z = (int16_t)((buf[5] << 8) | buf[4]) >> 4;

    convert_to_gs(data);
}
