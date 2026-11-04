#ifndef TC_UU_D_MKLK_MAP_H
#define TC_UU_D_MKLK_MAP_H
#include <stdint.h>

#define PANEL_SIZE      8
#define PANEL_ROWS      8
#define PANEL_COLS      40
#define NUM_PANELS      5
#define TOTAL_LEDS      320

extern const uint16_t map[PANEL_ROWS][PANEL_COLS];
#endif