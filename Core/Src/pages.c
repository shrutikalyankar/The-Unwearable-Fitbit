/**
 ******************************************************************************
 * @file           : pages.c
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : Various display pages for the LCD
 ******************************************************************************
*/

#include "pages.h"
#include "lcd.h"
#include "workout_images.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static PageID current_page = PAGE_TEXTS;
static TimeInfo current_time = {9, 50, 0};
static HealthStats current_stats = {0, 72, 98, 36.5f, 0.0f, 0.0f};

// text message storage
#define MAX_MESSAGES 4
#define MAX_MESSAGE_LENGTH 60
static char text_messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
static uint8_t message_count = 0;

// workout storage
static char old_workout[40] = "No workout";
static char current_workout[40] = "No workout";

static void DrawDigitalClock(void);
static void DrawAnalogClock(void);
static void DrawStatsPage(bool startup);
static void DrawTextsPage(void);
static void DrawWorkoutsPage(void);

// turn on a certain page
void Pages_Display(PageID page)
{
    current_page = page;

    // Clear screen
    LCD_Clear(COLOR_BLACK);
    LCD_Clear(COLOR_BLACK);
    HAL_Delay(100);

    switch(page)
    {
        case PAGE_CLOCK:
        	LCD_DrawString(10, 20, "CLOCK", COLOR_WHITE, COLOR_BLACK, 2);
			DrawAnalogClock();
			DrawDigitalClock();
			LCD_DrawRect(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, COLOR_MAGENTA); // adding a frame
            break;

        case PAGE_STATS:
        	LCD_DrawString(10, 20, "STATS", COLOR_WHITE, COLOR_BLACK, 2);
            DrawStatsPage(1); //startup
            break;

        case PAGE_TEXTS:
        	LCD_DrawString(10, 20, "TEXTS", COLOR_WHITE, COLOR_BLACK, 2);
            DrawTextsPage();
            break;

        case PAGE_WORKOUTS:
        	LCD_DrawString(10, 20, "WORKOUTS", COLOR_WHITE, COLOR_BLACK, 2);
        	DrawWorkoutsPage();
        	break;

        default:
            break;
    }
}


// Update time
void Pages_UpdateTime(uint8_t hours, uint8_t minutes, uint8_t seconds, bool update_analog)
{
    current_time.hours = hours;
    current_time.minutes = minutes;
    current_time.seconds = seconds;
    // If on clock page, update display
    if(current_page == PAGE_CLOCK)
    {
    	// just update digital normally
		DrawDigitalClock();
		if (update_analog){
			DrawAnalogClock();
		}
    }
}


// Update health stats
void Pages_UpdateStats(HealthStats *stats)
{
    if(stats != NULL && stats->calories != current_stats.calories)
    {
        current_stats = *stats;

        // If on stats page, update display
        if(current_page == PAGE_STATS)
        {
        	DrawStatsPage(0); // not startup
        }
    }
}

// Update current workout
void Pages_UpdateWorkout(const char *workout)
{
    if (workout == NULL) {
        return;
    }

    strncpy(old_workout, current_workout, 40 - 1);
    strncpy(current_workout, workout, 40 - 1);
    old_workout[40 - 1] = '\0';
    current_workout[40 - 1] = '\0';

    // If on workout page, refresh display
    if (current_page == PAGE_WORKOUTS) {
    	if (strcmp(old_workout,current_workout)!=0){
        	DrawWorkoutsPage();
		};
    }
}


// go to next page
void Pages_NextPage(void)
{
    current_page = (current_page + 1) % PAGE_COUNT;
    Pages_Display(current_page);
}


// go to prev page
void Pages_PrevPage(void)
{
    current_page = (current_page == 0) ? (PAGE_COUNT - 1) : (current_page - 1); // probably a better way to do this logic
    Pages_Display(current_page);
}


// Add a new text message
void Pages_AddText(const char *message)
{
    if (message == NULL || message[0] == '\0') {
        return;
    }

    // Shift messages if at max
    if (message_count >= MAX_MESSAGES) {
        for (int i = 0; i < MAX_MESSAGES - 1; i++) {
            strncpy(text_messages[i], text_messages[i + 1], MAX_MESSAGE_LENGTH - 1);
            text_messages[i][MAX_MESSAGE_LENGTH - 1] = '\0';
        }
        message_count = MAX_MESSAGES - 1;
    }

    // add in the new message at bottom
    strncpy(text_messages[message_count], message, MAX_MESSAGE_LENGTH - 1);
    text_messages[message_count][MAX_MESSAGE_LENGTH - 1] = '\0';
    message_count++;

    // If on texts page, refresh display
    if (current_page == PAGE_TEXTS) {
    	DrawTextsPage();
    }
}


// Clear all text messages
void Pages_ClearTexts(void)
{
    message_count = 0;
    for (int i = 0; i < MAX_MESSAGES; i++) {
        text_messages[i][0] = '\0';
    }

    // If on texts page, refresh display
    if (current_page == PAGE_TEXTS) {
        Pages_Display(PAGE_TEXTS);
    }
}

// Draw analog clock
static void DrawAnalogClock(void)
{
    int cx = ILI9341_WIDTH / 2;
    int cy = 120;
    int radius = 70;

    // idk why but I had to split the rectangle into two bc it was tweaking
    LCD_FillRect(cx-radius, cy-radius, radius, radius*2, COLOR_BLACK); // clear old time
    LCD_FillRect(cx, cy-radius, radius, radius*2, COLOR_BLACK); // clear old time

    // clock face circle
    for(int i = 0; i < 360; i += 2)
    {
        float angle_rad = i * 3.14159f / 180.0f;
        int x = cx + (int)(radius * cosf(angle_rad));
        int y = cy + (int)(radius * sinf(angle_rad));
        LCD_DrawPixel(x, y, COLOR_WHITE);
    }

    // get angles based on time
    float hour_angle = ((current_time.hours % 12) * 30 + current_time.minutes * 0.5f - 90) * 3.14159f / 180.0f;
    float minute_angle = (current_time.minutes * 6 - 90) * 3.14159f / 180.0f;

    // minute hand (longer)
    int min_x = cx + (int)((radius - 15) * cosf(minute_angle));
    int min_y = cy + (int)((radius - 15) * sinf(minute_angle));
    int dx = min_x - cx;
    int dy = min_y - cy;
    int steps = (dx*dx + dy*dy > 0) ? (int)sqrtf(dx*dx + dy*dy) : 1;
    for(int i = 0; i <= steps; i++)
    {
        int px = cx + (dx * i / steps);
        int py = cy + (dy * i / steps);
        LCD_DrawPixel(px, py, COLOR_CYAN);
        LCD_DrawPixel(px+1, py, COLOR_CYAN);
    }

    // hour hand (shorter)
    int hour_x = cx + (int)((radius - 35) * cosf(hour_angle));
    int hour_y = cy + (int)((radius - 35) * sinf(hour_angle));
    dx = hour_x - cx;
    dy = hour_y - cy;
    steps = (dx*dx + dy*dy > 0) ? (int)sqrtf(dx*dx + dy*dy) : 1;
    for(int i = 0; i <= steps; i++)
    {
        int px = cx + (dx * i / steps);
        int py = cy + (dy * i / steps);
        LCD_DrawPixel(px, py, COLOR_WHITE);
        LCD_DrawPixel(px+1, py, COLOR_WHITE);
        LCD_DrawPixel(px, py+1, COLOR_WHITE);
    }

    // center dot
    LCD_FillRect(cx - 2, cy - 2, 4, 4, COLOR_WHITE);
}

// Draw digital clock at bottom
static void DrawDigitalClock(void)
{
    char time_str[16];
    sprintf(time_str, "%02d:%02d:%02d", current_time.hours, current_time.minutes, current_time.seconds);

    // centering text
    int text_width = 8 * 2 * 6;  // 8 chars and the size for each one
    int x = (ILI9341_WIDTH - text_width) / 2;

    LCD_FillRect(x-1, 250, text_width+1, 15, COLOR_BLACK); // clear old time
    LCD_DrawString(x, 250, time_str, COLOR_CYAN, COLOR_BLACK, 2);
}

// Draw a 10x10 workout icon with scaling
static void DrawWorkoutIcon(int x, int y, const uint16_t *icon, uint16_t color, uint8_t scale)
{
    for (int row = 0; row < WORKOUT_IMG_HEIGHT; row++) {
        uint16_t row_data = icon[row];

        for (int col = 0; col < WORKOUT_IMG_WIDTH; col++) {
            // Check if this pixel should be drawn (bit is set)
            if (row_data & (1 << (WORKOUT_IMG_WIDTH - 1 - col))) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        LCD_DrawPixel(x + col * scale + sx, y + row * scale + sy, color);
                    }
                }
            }
        }
    }
}

// draw workout (right now just going to print it)
static void DrawWorkoutsPage(void)
{
    // centering text
    int text_width = strlen(current_workout) *3 * 6;  // 8 chars and the size for each one
    int x = (ILI9341_WIDTH - text_width) / 2;
    int y = (ILI9341_HEIGHT / 2) - 10;

    LCD_FillRect(0, y-1, ILI9341_WIDTH, 50 + 45, COLOR_BLACK); // clear old workout
    LCD_DrawString(x, y, current_workout, COLOR_CYAN, COLOR_BLACK, 3);

    // get workout icon
	const uint16_t *icon = NULL;

	if (strcmp(current_workout, "WeightLift") == 0) {
		icon = weightlift_icon;
	}
	else if (strcmp(current_workout, "Walking") == 0) {
		icon = walking_icon;
	}
	else if (strcmp(current_workout, "Plank") == 0) {
		icon = plank_icon;
	}
	else if (strcmp(current_workout, "JumpingJacks") == 0) {
		icon = jumpingjacks_icon;
	}
	else if (strcmp(current_workout, "Squats") == 0) {
		icon = squats_icon;
	}
	else if (strcmp(current_workout, "JumpRope") == 0) {
		icon = jumprope_icon;
	}

	// Draw if icon was found
	if (icon != NULL) {
		int icon_x = (ILI9341_WIDTH - WORKOUT_IMG_WIDTH * 4) / 2;
		int icon_y = y + 50;
		DrawWorkoutIcon(icon_x, icon_y, icon, COLOR_WHITE, 4);
	}

}

// Draw stats page. shows steps, heart rate, SPO2, temperature, Distance walked, calories
static void DrawStatsPage(bool startup)
{
    // 2 columns, 3 rows
    int col1_x = 30;
    int col2_x = 140;
    int start_y = 80;
    int row_height = 60;

    char buffer[16];

    if (startup){ // do the titles once on startup
    	LCD_DrawString(col1_x, start_y, "Steps", COLOR_WHITE, COLOR_BLACK, 1);
    	LCD_DrawString(col2_x, start_y, "Heart Rate", COLOR_WHITE, COLOR_BLACK, 1);
    	LCD_DrawString(col1_x, start_y + row_height, "SpO2", COLOR_WHITE, COLOR_BLACK, 1);
    	LCD_DrawString(col2_x, start_y + row_height, "Temp", COLOR_WHITE, COLOR_BLACK, 1);
    	LCD_DrawString(col1_x, start_y + row_height * 2, "Distance", COLOR_WHITE, COLOR_BLACK, 1);
    	LCD_DrawString(col2_x, start_y + row_height * 2, "Calories", COLOR_WHITE, COLOR_BLACK, 1);
    } else {
    	// clear old stats
		LCD_FillRect(0, start_y + 15, ILI9341_WIDTH, 15, COLOR_BLACK);
		LCD_FillRect(0, start_y + row_height + 15, ILI9341_WIDTH, 15, COLOR_BLACK);
		LCD_FillRect(0, start_y + row_height * 2 + 15, ILI9341_WIDTH, 15, COLOR_BLACK);
    }

    // Steps and HR
    sprintf(buffer, "%lu", (unsigned long)current_stats.steps);
    LCD_DrawString(col1_x, start_y + 15, buffer, COLOR_GREEN, COLOR_BLACK, 2);

    sprintf(buffer, "%d bpm", current_stats.heart_rate);
    LCD_DrawString(col2_x, start_y + 15, buffer, COLOR_RED, COLOR_BLACK, 2);

    // SpO2 and Temperature
    sprintf(buffer, "%d%%", current_stats.oxygen_level);
    LCD_DrawString(col1_x, start_y + row_height + 15, buffer, COLOR_CYAN, COLOR_BLACK, 2);

    sprintf(buffer, "%.1fC", current_stats.temperature);
    LCD_DrawString(col2_x, start_y + row_height + 15, buffer, COLOR_YELLOW, COLOR_BLACK, 2);

    // dist and calories
    sprintf(buffer, "%.1f m", current_stats.distance);
    LCD_DrawString(col1_x, start_y + row_height * 2 + 15, buffer, COLOR_MAGENTA, COLOR_BLACK, 2);

	sprintf(buffer, "%.2f", current_stats.calories);
	LCD_DrawString(col2_x, start_y + row_height * 2 + 15, buffer, COLOR_BLUE, COLOR_BLACK, 2);
}

// Draw texts page w/ green message bubbles
static void DrawTextsPage(void)
{
    LCD_FillRect(0, 40, ILI9341_WIDTH, ILI9341_HEIGHT - 40, COLOR_BLACK); // clear old texts

    if (message_count == 0) {
        // no messages
        LCD_DrawString(50, 100, "No messages yet", COLOR_WHITE, COLOR_BLACK, 1);
        return;
    }

    int y_pos = 245;
    int box_height = 50;
    int spacing = 15;

    for (int i = message_count - 1; i >= 0; i--) {
        if (text_messages[i][0] != '\0') {
            // background box
            LCD_FillRect(10, y_pos, 300, box_height, COLOR_GREEN);

            // triangle tail at bottom left
			int tail_x = 10;
			int tail_y = y_pos + box_height;

			for (int row = 0; row < 8; row++) {
				LCD_FillRect(tail_x, tail_y + row, 8 - row, 1, COLOR_GREEN);
			}

            // message text
            LCD_DrawString(15, y_pos + 5, text_messages[i], COLOR_BLACK, COLOR_GREEN, 2);

            y_pos -= box_height + spacing;
        }
    }
}
