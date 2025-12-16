/**
 ******************************************************************************
 * @file           : uart.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : UART2 initialization for bluetooth communication and
 * 					 display control
 ******************************************************************************
*/

#include "uart.h"
#include <stdarg.h>
#include <stdio.h>

/* Blocking RX*/
int getchar_polled(void) {
    while (!(USART2->SR & USART_SR_RXNE));
    return USART2->DR;
}

/* Blocking TX*/
void putchar_polled(int c) {
    while (!(USART2->SR & USART_SR_TXE));
    USART2->DR = c;
}

/* Non-blocking RX - returns -1 if no data available*/
int getchar_nonblocking(void) {
    if (USART2->SR & USART_SR_RXNE) {
        return USART2->DR;
    }
    return -1;
}


/////////// TEXTS /////////

#define LINE_BUFFER_SIZE 128 // max text length
static char line_buffer[LINE_BUFFER_SIZE];
static uint8_t line_index = 0;
static uint8_t line_ready = 0;


/* handle each char entered and add to growing text line*/
void UART_ProcessInput(void)
{
    int c = getchar_nonblocking();

    if (c == -1) {
        return;  // user didn't enter anything
    }

    // handle if they hit enter and finished the text
    if (c == '\r' || c == '\n') {
        if (line_index > 0) {
            line_buffer[line_index] = '\0';  // Null terminate
            line_ready = 1;

            // echo
            putchar_polled('\r');
            putchar_polled('\n');
        }
    }
    // if they did a backspace
    else if (c == 127 || c == '\b') {
        if (line_index > 0) {
            line_index--;

            // echo
            putchar_polled('\b');
            putchar_polled(' ');
            putchar_polled('\b');
        }
    }
    // normal text char
    else if (c >= 32 && c < 127) {
        if (line_index < LINE_BUFFER_SIZE - 1) {
            line_buffer[line_index++] = c;

            // echo
            putchar_polled(c);
        }
    }
}


/* check if a text is ready*/
int UART_TextReady(void)
{
    return line_ready;
}


/* Get the completed text (returns NULL if the person hasn't sent it yet)*/
char* UART_GetLine(void)
{
    if (!line_ready) {
        return NULL;
    }

    /* Reset for next line*/
    line_ready = 0;
    line_index = 0;

    return line_buffer;
}

/*SENDING STRINGS OUT*/

void sendString(const char *str) {
    while (*str) {
    	putchar_polled(*str++);
    }
}

/*prints string in green in uart */
void sendStringGreen(const char *str){
	sendString(ANSI_GREEN_BOLD);
	sendString(str);
	sendString(ANSI_RESET);
}

void sendCharGreen(uint8_t ch) {
    sendString(ANSI_GREEN_BOLD);
    putchar_polled(ch);
    sendString(ANSI_RESET);
}

/*Initializes UART2*/
void UART_Init(void)
{
    // clock enable for USART2 and GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA2 and PA3 setting to alternate functions
    GPIOA->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3);
    GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1);
    GPIOA->AFR[0] &= ~((0xF << 8) | (0xF << 12)); // clear the AFs
    GPIOA->AFR[0] |= (7 << 8) | (7 << 12); // set AF7

    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT_2 | GPIO_OTYPER_OT_3);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPDR2 | GPIO_PUPDR_PUPDR3);
    GPIOA->PUPDR |= (GPIO_PUPDR_PUPDR2_0 | GPIO_PUPDR_PUPDR3_0);  // Pull-up

    // stop UART for configuring
    USART2->CR1 &= ~USART_CR1_UE;

    // Set baud rate to 9600
    // USART2 is on APB1
    USART2->BRR = 0x9C4;

    // UART settings
    // 8 data bits, 1 stop bit, no parity
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    // enable UART, TX, RX, RXNE interrupts
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}
