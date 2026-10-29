#include "graphics.h"
#include <string.h>
#include "stm32h7xx_hal.h"
#include "target.h"

static const uint8_t needle_down[8][8] = {
    {1, 1, 0, 0, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
};

static const uint8_t needle_up[8][8] = {
    {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 1, 1},
};

static const uint8_t needle_left[8][8] = {
    {0, 0, 0, 0, 0, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 1, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1}, {0, 0, 0, 0, 0, 0, 1, 1},
};

static const uint8_t needle_right[8][8] = {
    {1, 1, 0, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 1},
    {0, 0, 0, 0, 0, 1, 1, 1}, {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 0, 0, 0},
};

static const uint8_t buff[8][8] = {
    {1, 1, 0, 0, 0, 0, 1, 1}, {1, 1, 1, 0, 0, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}, {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 0, 0, 1, 1, 1}, {1, 1, 0, 0, 0, 0, 1, 1},
};

static const uint8_t tower[8][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 1, 1}, {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1},
};

static const uint8_t center[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0},
};

static void DrawPatternOverlay(LEDStrip_t* strip,
                               const uint8_t pattern[8][8],
                               Color_t color) {
  for (uint8_t y = 0; y < 8; y++) {
    for (uint8_t x = 0; x < 8; x++) {
      if (pattern[y][x]) {
        strip->panel[y][x].r = color.r;
        strip->panel[y][x].g = color.g;
        strip->panel[y][x].b = color.b;
        strip->panel[y][x].enable = 1;
      }
    }
  }
}

void Needle_Up_WithCenter(LEDStrip_t* strip,
                          Color_t fill,
                          Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, needle_up, fill);
  Target_LoadLEDPtr(strip);
}

void Needle_Down_WithCenter(LEDStrip_t* strip,
                            Color_t fill,
                            Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, needle_down, fill);
  Target_LoadLEDPtr(strip);
}

void Needle_Left_WithCenter(LEDStrip_t* strip,
                            Color_t fill,
                            Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, needle_left, fill);
  Target_LoadLEDPtr(strip);
}

void Needle_Right_WithCenter(LEDStrip_t* strip,
                             Color_t fill,
                             Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, needle_right, fill);
  Target_LoadLEDPtr(strip);
}

void Tower_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, tower, fill);
  Target_LoadLEDPtr(strip);
}

void Buff_WithCenter(LEDStrip_t* strip, Color_t fill, Color_t center_color) {
  memset(strip->panel, 0, sizeof(strip->panel));
  DrawPatternOverlay(strip, center, center_color);
  DrawPatternOverlay(strip, buff, fill);
  Target_LoadLEDPtr(strip);
}