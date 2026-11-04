#ifndef WATER_FLOW_H
#define WATER_FLOW_H
#include <stdint.h>
#include "color.h"

void WaterFlow_Init(void);
void WaterFlow_StartArrow(uint16_t leaf, Color_t fill);
void WaterFlow_Stop(uint16_t leaf);
void WaterFlow_Step(void);
void WaterFlow_Display(uint16_t leaf);
uint8_t WaterFlow_IsActive(void);
#endif