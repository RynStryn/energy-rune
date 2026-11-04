#include "led_panel.h"
#include <string.h>
#include "map.h"
#include "ws2812.h"

static Led_t g_virtual_panel[PANEL_ROWS][PANEL_COLS];
static uint8_t g_grb_buffer[TOTAL_LEDS * 3];

void Panel_Init(void)
{
    memset(g_virtual_panel, 0, sizeof(g_virtual_panel));
    memset(g_grb_buffer, 0, sizeof(g_grb_buffer));
    WS2812_Init();
}

void Panel_SetLed(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x >= PANEL_COLS || y >= PANEL_ROWS)
        return;
    g_virtual_panel[y][x].r = r;
    g_virtual_panel[y][x].g = g;
    g_virtual_panel[y][x].b = b;
    g_virtual_panel[y][x].enable = true;
}

void Panel_SetLedEnable(uint8_t x, uint8_t y, bool en)
{
    if (x >= PANEL_COLS || y >= PANEL_ROWS)
        return;
    g_virtual_panel[y][x].enable = en;
}

void Panel_Clear(void) { memset(g_virtual_panel, 0, sizeof(g_virtual_panel)); }

void Panel_load2led(void)
{
    memset(g_grb_buffer, 0, sizeof(g_grb_buffer));

    for (uint8_t y = 0; y < PANEL_ROWS; y++)
    {
        for (uint8_t x = 0; x < PANEL_COLS; x++)
        {
            if (g_virtual_panel[y][x].enable)
            {
                uint16_t idx = map[y][x];
                g_grb_buffer[idx * 3 + 0] = g_virtual_panel[y][x].g;
                g_grb_buffer[idx * 3 + 1] = g_virtual_panel[y][x].r;
                g_grb_buffer[idx * 3 + 2] = g_virtual_panel[y][x].b;
            }
        }
    }
    WS2812_SendFrame(g_grb_buffer);
}
