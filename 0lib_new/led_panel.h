#ifndef TC_UU_D_MKLK_LED_PANEL_H
#define TC_UU_D_MKLK_LED_PANEL_H
#include <stdbool.h>
#include <stdint.h>
typedef struct
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    bool enable;
} Led_t;

void Panel_Init(void);
void Panel_SetLed(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void Panel_SetLedEnable(uint8_t x, uint8_t y, bool en);
void Panel_Clear(void);
void Panel_load2led(void);
#endif
