/**
 ******************************************************************************
 * @file           : uart.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : UART2 initialization for bluetooth communication and
 * 					 display control
 ******************************************************************************
*/

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

// ansi color codes
#define ANSI_RESET       "\x1b[0m"
#define ANSI_BOLD        "\x1b[1m"
#define ANSI_GREEN_BOLD  "\x1b[1;32m"
// Haven't used these yet but just in case
#define ANSI_BLUE_BOLD   "\x1b[1;34m"
#define ANSI_CYAN_BOLD   "\x1b[1;36m"

void UART_Init(void);
int getchar_nonblocking(void);
int getchar_polled(void);
void putchar_polled(int c);

// text funcs
void UART_ProcessInput(void);
int UART_TextReady(void);
char* UART_GetLine(void);

// sending strings
void sendString(const char *str);
void sendStringGreen(const char *str);
void sendCharGreen(uint8_t ch);

#endif
