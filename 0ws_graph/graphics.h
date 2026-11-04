#ifndef TC_UU_D_MKLK_GRAPHICS_H
#define TC_UU_D_MKLK_GRAPHICS_H

#include <stdint.h>
#include "color.h"
#include "target.h"

void Needle_Up_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);
void Needle_Down_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);
void Needle_Left_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);
void Needle_Right_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);
void Tower_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);
void Buff_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center);

#endif  // TC_UU_D_MKLK_GRAPHICS_H