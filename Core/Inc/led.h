/**
 ******************************************************************************
 * @file           : led.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : helper file. Was only used during development phase for
 * 					 debugging
 ******************************************************************************
*/
#ifndef LED_H
#define LED_H

#include "stm32f411xe.h"

// ACTIVE-HIGH LED CONTROL (for STM32F411E Discovery)
#define LED_ON(port, pin)   ((port)->BSRR = (1 << (pin)))           // set bit
#define LED_OFF(port, pin)  ((port)->BSRR = (1 << ((pin) + 16)))    // reset bit

// LED pin definitions
#define LED_GREEN   12
#define LED_ORANGE  13
#define LED_RED     14
#define LED_BLUE    15

#endif
