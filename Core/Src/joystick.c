/**
 ******************************************************************************
 * @file           : joystick.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Initializing ADC and DMA. Reading Joystick values
 * 					 Joystick handler to control display pages on the LCD
 ******************************************************************************
*/
#include "joystick.h"
#include "stm32f411xe.h"
#include "pages.h"
#include <stdlib.h>
#include "lcd.h"
#include "uart.h"
#define CENTER_X   2028
#define CENTER_Y   2027

#define DEADZONE   250     // small zone around the center

volatile uint16_t adc_buf[2];  // VRx, VRy values
volatile int lastDirection = 0;	//flag to track which joystick direction

/*Initializes DMA and ADC*/
void Joystick_Init(void){
    // Enable GPIOA clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // PA0, PA1 = analog
    GPIOA->MODER |= (3 << (0*2)) | (3 << (1*2));

    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Enable DMA2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Configure DMA2 Stream0
    DMA2_Stream0->CR = 0;
    DMA2_Stream0->PAR = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)adc_buf;  //stores the values in the array
    DMA2_Stream0->NDTR = 2; // 2 channels
    DMA2_Stream0->CR =
        (0 << 25) |      // CHSEL = 0
        DMA_SxCR_PL_1 |  // priority high
        DMA_SxCR_MSIZE_0 | // memory 16-bit
        DMA_SxCR_PSIZE_0 | // peripheral 16-bit
        DMA_SxCR_MINC |    // memory increment
        DMA_SxCR_CIRC |    // circular
        DMA_SxCR_EN;

    // Configure ADC1
    ADC->CCR |= ADC_CCR_ADCPRE_0; // PCLK2/4
    ADC1->CR1 = 0;
    ADC1->CR2 = ADC_CR2_DMA | ADC_CR2_DDS | ADC_CR2_CONT | ADC_CR2_ADON;

    // Two channels: IN0, IN1
    ADC1->SQR1 = (1 << 20); // L = 1 → 2 conversions
    ADC1->SQR3 = (0 << 0) | (1 << 5); // SQ1=ch0, SQ2=ch1

    ADC1->SMPR2 |= (3 << (0*3)) | (3 << (1*3)); // sample time

    // Start ADC
    ADC1->CR2 |= ADC_CR2_SWSTART;
}

/*Get joystick position*/
void Joystick_Read(uint16_t *x, uint16_t *y){
    *x = adc_buf[0];
    *y = adc_buf[1];
}

/*Control display pages based on joystick readings*/
void HandleJoystick(int x, int y)
{
    int dx = x - CENTER_X;
    int dy = y - CENTER_Y;

    //get absolute values
    int ax = abs(dx);
    int ay = abs(dy);

    // CENTER = do nothing
    if (ax < DEADZONE && ay < DEADZONE) {
    	lastDirection = 0;
        return;
    }

    if (x < 5 && y < 5 && lastDirection == 0) {
		// to the left, rising edge
		lastDirection = 1;
		Pages_PrevPage();
		return;
	}
	if (x > 3000 && y > 3000 && lastDirection == 0){
		// to the right, rising edge
		lastDirection = 2;
		Pages_NextPage();
		return;
	}
}


