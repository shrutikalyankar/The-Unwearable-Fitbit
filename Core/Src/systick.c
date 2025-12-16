/*
 * systick.c
 *
 *  Created on: Nov 24, 2025
 *      Author: dafe2002
 */


#include "stm32f4xx.h"
#include "pages.h"
#include <stdbool.h>


// Global time variables
static volatile uint32_t milliseconds = 0;
static volatile uint8_t hours = 9;
static volatile uint8_t minutes = 52;
static volatile uint8_t seconds = 0;


// init SysTick to trigger every 1ms
void SysTick_Init(void)
{
    // 100 MHz system clock
	// 100,000 cycles for 1 ms
    SysTick->LOAD = 100000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | // Enable interrupt
                    SysTick_CTRL_ENABLE_Msk;
}


// interrupt to update the clock every 1ms
void SysTick_Handler(void)
{
    milliseconds++;
    HAL_IncTick(); // keeping what it already did so we can still use built in time funcs

    // every 1 s, update
    if (milliseconds >= 1000)
    {
    	bool update_analog = 0;
        milliseconds = 0;
        seconds++;

        if (seconds >= 60)
        {
            seconds = 0;
            minutes++;
            update_analog = 1; // now that the minutes changed, we need to update the analog clock

            if (minutes >= 60)
            {
                minutes = 0;
                hours++;

                if (hours >= 24)
                {
                    hours = 0;
                }
            }
        }

        // Update the clock display
        Pages_UpdateTime(hours, minutes, seconds, update_analog);
    }
}


// Set the current time. for on startup
void Clock_SetTime(uint8_t h, uint8_t m, uint8_t s)
{
    __disable_irq(); // disable interrupts
    hours = h;
    minutes = m;
    seconds = s;
    milliseconds = 0;
    __enable_irq(); // reenable

    // update the display w the new time
    Pages_UpdateTime(hours, minutes, seconds, 1); // always update analog in this case
}
