//
// Created by 31466 on 2026/2/5.
//

#ifndef FANCY_MOTOR_SOFTWARE_CONFIG_H
#define FANCY_MOTOR_SOFTWARE_CONFIG_H

#define SMALL_RUNE 0
#define BIG_RUNE 1

#define generationTimeLimit 1000
#define FlashInterval 25
#define TIME_OUT 2500
#define BONUS_WINDOW_TIME 1000
#define successShowTime 5000


typedef struct gError {
    uint16_t GenerationLoop;
    uint16_t updateTarget;
    uint16_t RNGwrong;
} gError_t;

extern gError_t gError;
#endif //FANCY_MOTOR_SOFTWARE_CONFIG_H