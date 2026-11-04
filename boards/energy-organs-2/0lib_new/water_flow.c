#include "water_flow.h"
#include <string.h>
#include "map.h"
#include "ws2812b.h"

static Color_t g_fb[PANEL_ROWS][PANEL_COLS];
static uint8_t g_active = 0;

static const uint8_t arrow_matrix[PANEL_ROWS][PANEL_COLS] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
     0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
     1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
     1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1,
     1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1,
     1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1,
     0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void WaterFlow_Init(void) {
  memset(g_fb, 0, sizeof(g_fb));
  g_active = 0;
}

void WaterFlow_StartArrow(uint16_t leaf, Color_t fill) {
  for (uint8_t y = 0; y < PANEL_ROWS; y++) {
    for (uint8_t x = 0; x < PANEL_COLS; x++) {
      g_fb[y][x] = arrow_matrix[y][x] ? fill : COLOR_BLACK;
    }
  }
  g_active = 1;
  WaterFlow_Display(leaf);
}

void WaterFlow_Stop(uint16_t leaf) {
  g_active = 0;
  memset(g_fb, 0, sizeof(g_fb));
  my_ws2812_set_all_off(leaf);
  transfer(leaf);
}

void WaterFlow_Step(void) {
  if (!g_active)
    return;
  for (uint8_t y = 0; y < PANEL_ROWS; y++) {
    Color_t last = g_fb[y][PANEL_COLS - 1];
    for (int16_t x = PANEL_COLS - 1; x > 0; x--) {
      g_fb[y][x] = g_fb[y][x - 1];
    }
    g_fb[y][0] = last;
  }
}

void WaterFlow_Display(uint16_t leaf) {
  if (!g_active)
    return;
  my_ws2812_set_all_off(leaf);
  for (uint8_t y = 0; y < PANEL_ROWS; y++) {
    for (uint8_t x = 0; x < PANEL_COLS; x++) {
      Color_t c = g_fb[y][x];
      if (c.r || c.g || c.b) {
        my_ws2812_1_set(leaf, map[y][x] + 1, c.r, c.g, c.b);
      }
    }
  }
  transfer(leaf);
}

uint8_t WaterFlow_IsActive(void) {
  return g_active;
}