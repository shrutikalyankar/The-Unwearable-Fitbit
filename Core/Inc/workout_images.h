/**
 ******************************************************************************
 * @file           : workout_images.h
 * @author         : Daphne Felt, Shruti Kalyankar
 * @brief          : bitmaps for animations for different workouts
 ******************************************************************************
*/

#ifndef WORKOUT_IMAGES_H
#define WORKOUT_IMAGES_H

#include <stdint.h>

// 10x10 pixels
#define WORKOUT_IMG_WIDTH  10
#define WORKOUT_IMG_HEIGHT 10

// WeightLifting
static const uint16_t weightlift_icon[10] = {
    0b1100000011,  // Barbell weights
    0b1100000011,  //
    0b1111111111,  // Bar
    0b1111111111,  //
    0b1101001011,  //
    0b1101001011,  //
    0b0001111000,  //
    0b0000110000,  // Person lifting
    0b0001001000,  //
    0b0001001000,  //
};


// Walking
static const uint16_t walking_icon[10] = {
    0b0001111000,  // Head
    0b0011111100,  //
    0b0001111000,  //
    0b0000110000,  // Body
    0b0001111100,  // Arms
    0b0010110100,  //
    0b0100110100,  //
    0b0001001000,  // Legs walking
    0b0001001000,  //
    0b0010000100,  //
};


// Plank icon (person horizontal)
static const uint16_t plank_icon[10] = {
    0b0000000000,  //
    0b0000000000,  // Head
    0b0000000011,  //
    0b1111111111,  // Body horizontal
    0b1111111111,  //
    0b1000000100,  // Arms supporting
    0b0000000000,  //
    0b0000000000,  //
    0b0000000000,  //
    0b0000000000,  //
};


// JumpingJacks
static const uint16_t jumpingjacks_icon[10] = {
    0b0001111000,  // Head
    0b1011111101,  //
    0b1001111001,  //
    0b1111111111,  // Arms spread wide
    0b0000110000,  //
    0b0000110000,  //
    0b0000110000,  // Body
    0b0001111000,  // Legs spread
    0b0011001100,  //
    0b0110000110,  //
};


// Squats
static const uint16_t squats_icon[10] = {
    0b0001111000,  // Head
    0b0011111100,  //
    0b0001111000,  //
    0b0111111110,  // Arms forward
    0b0111111110,  //
    0b0001111000,  // Body
    0b0011111100,  // Squatting
    0b0111111110,  //
    0b1110000111,  // Bent legs
    0b1100000011,  //
};


// JumpRope
static const uint16_t jumprope_icon[10] = {
    0b0000000000,  //
    0b0000110000,  //
    0b0000110000,  // Head
    0b0000110000,  //
    0b0011111100,  // Arms
    0b1101111011,  //
    0b1001001001,  // Body
    0b1001001001,  // Jumping
    0b1000000001,  //
    0b0111111110,  // Rope below
};


#endif

