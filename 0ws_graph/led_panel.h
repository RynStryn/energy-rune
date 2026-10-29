#ifndef TC_UU_D_MKLK_LED_PANEL_H
#define TC_UU_D_MKLK_LED_PANEL_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  bool enable;
} Led_t;

#endif