#ifndef FANCY_MOTOR_SOFTWARE_CONFIG_H
#define FANCY_MOTOR_SOFTWARE_CONFIG_H

#include <stdint.h>

#define SMALL_RUNE 0
#define BIG_RUNE 1

#define CPU_FREQ_MHZ     240
#define US(x)            ((uint32_t)((x) * CPU_FREQ_MHZ))
#define MS(x)            US((x) * 1000)

#define SwITCH_PERIOD     MS(500)
#define LOGIC_PERIOD      MS(1)

#define LED_REFRESH_PERIOD   MS(100)
#define LED_PROTECTION_MS    MS(30)
#define LED_ANIM_PERIOD      MS(100)

#define TIME_OUT_MS          MS(2500)//800,320
#define BONUS_WINDOW_MS      MS(1000)//
#define SUCCESS_SHOW_MS      MS(5000)

#define generationTimeLimit  1000

typedef struct gError {
  uint16_t GenerationLoop;
  uint16_t updateTarget;
  uint16_t RNGwrong;
} gError_t;

extern gError_t gError;

#endif