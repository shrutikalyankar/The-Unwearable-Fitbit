/**
 ******************************************************************************
 * @file           : lcd.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : ILI9341 LCD initialization and command functions
 * 					 for LCD control
 ******************************************************************************
*/
#include "lcd.h"
#include "spi.h"
#include "font.h"

/*Sends a command byte to the controller over SPI*/
static void LCD_SendCommand(uint8_t cmd)
{
    LCD_DC_LOW();
    LCD_CS_LOW();
    SPI1_WriteByte(cmd);
    LCD_CS_HIGH();
}

/*Sends single byte data to the LCD*/
static void LCD_SendData(uint8_t data)
{
	LCD_DC_LOW();
    LCD_CS_LOW();
    SPI1_WriteByte(data);
    LCD_CS_HIGH();
}

/*Sends 16bit data to LCD*/
static void LCD_SendData16(uint16_t data)
{
    LCD_SendData(data >> 8);
    LCD_SendData(data & 0xFF);
}

/*Sets the drawing region of the LCD for future writes*/
static void LCD_SetAddressWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    LCD_SendCommand(ILI9341_COLADDR);
    LCD_SendData(x >> 8); LCD_SendData(x & 0xFF);
    LCD_SendData((x+w-1) >> 8); LCD_SendData((x+w-1) & 0xFF);

    LCD_SendCommand(ILI9341_ROWADDR);
    LCD_SendData(y >> 8); LCD_SendData(y & 0xFF);
    LCD_SendData((y+h-1) >> 8); LCD_SendData((y+h-1) & 0xFF);

    LCD_SendCommand(ILI9341_MEMORYWRITE);
}

/*Configures CS, DC, RST pins as outputs*/

static void LCD_GPIO_Init(void)
{
    // Enable GPIO clocks
    //if(LCD_DC_PORT == GPIOB || LCD_RST_PORT == GPIOB)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // For PE0 (CS)

    // Configure as output
    LCD_CS_PORT->MODER &= ~(3<<(LCD_CS_PIN*2));
    LCD_CS_PORT->MODER |=  (1<<(LCD_CS_PIN*2));

    LCD_DC_PORT->MODER &= ~(3<<(LCD_DC_PIN*2));
    LCD_DC_PORT->MODER |=  (1<<(LCD_DC_PIN*2));

    LCD_RST_PORT->MODER &= ~(3<<(LCD_RST_PIN*2));
    LCD_RST_PORT->MODER |=  (1<<(LCD_RST_PIN*2));

    // Push-pull
    LCD_CS_PORT->OTYPER &= ~(1<<LCD_CS_PIN);
    LCD_DC_PORT->OTYPER &= ~(1<<LCD_DC_PIN);
    LCD_RST_PORT->OTYPER &= ~(1<<LCD_RST_PIN);

    // No pull-up/pull-down
    LCD_CS_PORT->PUPDR &= ~(3<<(LCD_CS_PIN*2));
    LCD_DC_PORT->PUPDR &= ~(3<<(LCD_DC_PIN*2));
    LCD_RST_PORT->PUPDR &= ~(3<<(LCD_RST_PIN*2));

    // Set CS high, DC/data high, RST high
    LCD_CS_HIGH();
    LCD_DC_DATA();
    LCD_RST_HIGH();
}

/*Initializes, resets LCD, sets orientation*/
void LCD_Init(void)
{
    LCD_GPIO_Init();

    // Reset LCD
    LCD_RST_LOW();
    for(volatile int i=0;i<2000000;i++);
    LCD_RST_HIGH();
    for(volatile int i=0;i<2000000;i++);

    // Start initialization commands
    LCD_SendCommand(ILI9341_SWRESET);
    for(volatile int i=0;i<2000000;i++);

    LCD_SendCommand(ILI9341_PIXELFORMAT);
    LCD_SendData(0x55); // 16-bit

    LCD_SendCommand(ILI9341_MADCTL);
    LCD_SendData(0x48);

    LCD_SendCommand(0x11); // Sleep out
    for(volatile int i=0;i<2000000;i++);

    LCD_SendCommand(ILI9341_DISPON); // Display on
}

/*Fills entire screen with a single colour given as input */
void LCD_Clear(uint16_t color)
{
    LCD_FillRect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, color);
}

/* To draw a single pixel at given co-ordinat*/
void LCD_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_SetAddressWindow(x, y, 1, 1);
    LCD_SendData16(color);
}

/*Draws the outline of a rectangle */
void LCD_DrawRect(int x, int y, int w, int h, uint16_t color)
{
    for(int i = x; i < x + w; i++)
    {
        LCD_DrawPixel(i, y, color);
        LCD_DrawPixel(i, y + h - 1, color);
    }
    for(int j = y; j < y + h; j++)
    {
        LCD_DrawPixel(x, j, color);
        LCD_DrawPixel(x + w - 1, j, color);
    }
}

/*Fills the rectangle with the given colour*/
void LCD_FillRect(int x, int y, int w, int h, uint16_t color)
{
    LCD_SetAddressWindow(x, y, w, h);

    LCD_DC_DATA();
    LCD_CS_LOW();

    uint32_t total = w * h;
    while(total--)
        SPI1_WriteByte(color >> 8), SPI1_WriteByte(color & 0xFF);

    LCD_CS_HIGH();
}

// Draw a single character at position (x,y)
// for size, it's like a scalar. 1 is normal, 2 is double, etc
void LCD_DrawChar(int x, int y, char c, uint16_t color, uint16_t bg, uint8_t size)
{
    if(c < 32 || c > 126) return; // make sure it's printable ascii char
    const uint8_t *glyph = font5x7[c - 32];
    for(int col = 0; col < FONT_WIDTH; col++)
    {
        uint8_t line = glyph[col];

        for(int row = 0; row < FONT_HEIGHT; row++)
        {
            uint16_t pixel_color = (line & 0x01) ? color : bg;

            if(size == 1)
            {
                LCD_DrawPixel(x + col, y + row, pixel_color);
            }
            else
            {
                // Draw scaled pixel
                LCD_FillRect(x + col * size, y + row * size, size, size, pixel_color);
            }

            line >>= 1;
        }
    }
}

// Draw a string at position (x,y)
void LCD_DrawString(int x, int y, const char* str, uint16_t color, uint16_t bg, uint8_t size)
{
    int cursor_x = x;
    while(*str)
    {
        if(*str == '\n') // newline
        {
            cursor_x = x;
            y += (FONT_HEIGHT + 1) * size;
        }
        else
        {
            // Draw char and move cursor fwd
            LCD_DrawChar(cursor_x, y, *str, color, bg, size);
            cursor_x += (FONT_WIDTH + 1) * size; // adding a pixel btw chars
            if(cursor_x + (FONT_WIDTH * size) > ILI9341_WIDTH) // handle line wrap too
            {
                cursor_x = x;
                y += (FONT_HEIGHT + 1) * size;
            }
        }
        str++;
    }
}

/*Animation for the opening screen, comes up on reboot*/
void OpeningScreen_Slide(void)
{
    LCD_Clear(COLOR_GREEN);

    for(int x=-240; x<=0; x+=8) {
        LCD_FillRect(x, 140, 240, 40, COLOR_BLUE);
        HAL_Delay(10000);
        LCD_FillRect(x-8, 140, 8, 40, COLOR_GREEN); // erase trail
    }

    LCD_DrawString(30, 150, "THE UNWEARABLE", COLOR_WHITE, COLOR_BLUE, 2.5);
    HAL_Delay(80000);
}
