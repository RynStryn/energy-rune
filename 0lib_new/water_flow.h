#ifndef WATER_FLOW_H
#define WATER_FLOW_H
#include <stdint.h>
#include "color.h"

void WaterFlow_Init(void);
void WaterFlow_Start(Color_t fill);
void WaterFlow_StartArrow(Color_t fill);
void WaterFlow_Stop(void);
void WaterFlow_Step(void);
void WaterFlow_Display(void);
uint8_t WaterFlow_IsActive(void);
#endif