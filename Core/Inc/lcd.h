/**
 ******************************************************************************
 * @file           : lcd.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : ILI9341 LCD initialization and command functions
 * 					 for LCD control
 ******************************************************************************
*/
#ifndef LCD_H
#define LCD_H

#include "stm32f4xx.h"
#include <stdint.h>

/* ILI9341 Commands */
#define ILI9341_SWRESET      0x01		//software reset to reinitialize lcd
#define ILI9341_DISPON       0x29		//light up the screen
#define ILI9341_MADCTL       0x36		//memory access ctrl. mx=1, bgr = 1
#define ILI9341_COLADDR      0x2A		//defines horizontal range where pixel data will be written
#define ILI9341_ROWADDR      0x2B		//defines vertical range where pixel data will be written
#define ILI9341_MEMORYWRITE  0x2C		//
#define ILI9341_PIXELFORMAT  0x3A		//determines bits used by each pixel

//pixels
#define ILI9341_WIDTH        240
#define ILI9341_HEIGHT       320

/* GPIO Pins */
#define LCD_CS_PORT      GPIOE
#define LCD_CS_PIN       0 // PE0

#define LCD_DC_PORT      GPIOB
#define LCD_DC_PIN       7

#define LCD_RST_PORT     GPIOB
#define LCD_RST_PIN      8

/*Macros to set gpio pins to high or low*/
#define LCD_CS_LOW()     (LCD_CS_PORT->BSRR = (1 << (LCD_CS_PIN + 16)))
#define LCD_CS_HIGH()    (LCD_CS_PORT->BSRR = (1 << LCD_CS_PIN))

#define LCD_DC_LOW() (LCD_DC_PORT->BSRR = (1 << (LCD_DC_PIN + 16)))
#define LCD_DC_DATA()    (LCD_DC_PORT->BSRR = (1 << LCD_DC_PIN))

#define LCD_RST_LOW()    (LCD_RST_PORT->BSRR = (1 << (LCD_RST_PIN + 16)))
#define LCD_RST_HIGH()   (LCD_RST_PORT->BSRR = (1 << LCD_RST_PIN))

void LCD_Init(void);
void LCD_Clear(uint16_t color);
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawRect(int x, int y, int w, int h, uint16_t color);
void LCD_FillRect(int x, int y, int w, int h, uint16_t color);

/* Basic colors, can be adjusted */
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_CYAN        0x07FF
#define COLOR_MAGENTA     0xF81F

// funcs for adding text
void LCD_DrawChar(int x, int y, char c, uint16_t color, uint16_t bg, uint8_t size);
void LCD_DrawString(int x, int y, const char* str, uint16_t color, uint16_t bg, uint8_t size);

#endif
