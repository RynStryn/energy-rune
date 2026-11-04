#ifndef TC_UU_D_MKLK_WS2812_H
#define TC_UU_D_MKLK_WS2812_H

#include <stdint.h>

#define WS2812_NUM_LEDS 320

void WS2812_Init(void);
void WS2812_SendFrame(const uint8_t* grb_array);

#endif
