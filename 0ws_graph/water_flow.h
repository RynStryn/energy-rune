#ifndef WATER_FLOW_H
#define WATER_FLOW_H
#include <stdint.h>
#include "color.h"
#include "target.h"

void WaterFlow_Init(LEDStrip_t* strip);
void WaterFlow_Start(LEDStrip_t* strip, Color_t fill);
void WaterFlow_StartArrow(LEDStrip_t* strip, Color_t fill);
void WaterFlow_Stop(LEDStrip_t* strip);
void WaterFlow_Step(LEDStrip_t* strip);
void WaterFlow_Display(LEDStrip_t* strip);
uint8_t WaterFlow_IsActive(LEDStrip_t* strip);
#endif