/*
 * systick.h
 *
 *  Created on: Nov 24, 2025
 *      Author: dafe2002
 */

void SysTick_Init(void);
void SysTick_Handler(void);
void Clock_SetTime(uint8_t h, uint8_t m, uint8_t s);
