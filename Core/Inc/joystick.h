/**
 ******************************************************************************
 * @file           : joystick.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Initializing ADC and DMA. Reading Joystick values
 * 					 Joystick handler to control display pages on the LCD
 ******************************************************************************
*/
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

/*debugging values for calibrating joystick*/
//center values are x= 2028 and y=2027
//when I move it directly right, x = 4095, y = 4093
//when I move it directly left, x = 2, y = 2
//straight up, nothing lights up but x = 2028, y = 2028
//straight down, nothing lights up but x = 2030, y = 2030

extern volatile int currentPage;
extern volatile int nextPage;

void Joystick_Init(void);
void Joystick_Read(uint16_t *x, uint16_t *y);
void HandleJoystick(int x, int y);

#endif
