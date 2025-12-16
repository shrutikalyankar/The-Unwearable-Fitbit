/**
 ******************************************************************************
 * @file           : pages.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Various display pages for the LCD
 ******************************************************************************
*/

#ifndef PAGES_H
#define PAGES_H

#include <stdint.h>
#include <stdbool.h>

// assigning number IDs for each page
typedef enum {
    PAGE_CLOCK = 0,
    PAGE_STATS = 1,
	PAGE_TEXTS = 2,
	PAGE_WORKOUTS = 3,
	/// ADD MORE PAGES HERE
    PAGE_COUNT = 4 // total num of pages
} PageID;

// Health stats struct
typedef struct {
    uint32_t steps;
    uint8_t heart_rate;
    uint8_t oxygen_level;
    float temperature;
    float distance;
    float calories;
} HealthStats;

// Time struct
typedef struct {
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} TimeInfo;

void Pages_Display(PageID page);
void Pages_UpdateTime(uint8_t hours, uint8_t minutes, uint8_t seconds, bool update_analog);
void Pages_UpdateStats(HealthStats *stats);
void Pages_NextPage(void);
void Pages_PrevPage(void);

// texting
void Pages_AddText(const char *message);
void Pages_ClearTexts(void);

// workout
void Pages_UpdateWorkout(const char *workout);

#endif

