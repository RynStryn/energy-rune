//
// Created by RynStryn on 2026/5/12.
//
#include "graphics.h"
#include "led_panel.h"


static const uint8_t needle_down[8][8] = {
    {1,1,0,0,0,0,1,1},
    {1,1,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
};

static const uint8_t needle_up[8][8] = {
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,1,1},
    {1,1,0,0,0,0,1,1},
};

static const uint8_t needle_left[8][8] = {
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0},
    {1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,1},
    {0,0,0,0,0,0,1,1},
};

static const uint8_t needle_right[8][8] = {
    {1,1,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,0,1,1,1},
    {0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0},
};

static const uint8_t buff[8][8] = {
    {1,1,0,0,0,0,1,1},
    {1,1,1,0,0,1,1,1},
    {0,1,1,1,1,1,1,0},
    {0,0,1,0,0,1,0,0},
    {0,0,1,0,0,1,0,0},
    {0,1,1,1,1,1,1,0},
    {1,1,1,0,0,1,1,1},
    {1,1,0,0,0,0,1,1},
};

static const uint8_t tower[8][8] = {
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,0,0,0,0,1,1},
    {1,1,0,0,0,0,1,1},
    {1,1,0,0,0,0,1,1},
    {1,1,0,0,0,0,1,1},
    {1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1},
};

static const uint8_t center[8][8] = {
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
};

static void DrawPatternOverlay(const uint8_t pattern[8][8], Color_t color)
{
    for (uint8_t y = 0; y < 8; y++) {
        for (uint8_t x = 0; x < 8; x++) {
            if (pattern[y][x]) {
                Panel_SetLed(x, y, color.r, color.g, color.b);
            }
        }
    }
}

void Needle_Up_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(needle_up, fill_color);
    Panel_load2led();
}

void Needle_Down_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(needle_down, fill_color);
    Panel_load2led();
}

void Needle_Left_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(needle_left, fill_color);
    Panel_load2led();
}

void Needle_Right_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(needle_right, fill_color);
    Panel_load2led();
}

void Tower_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(tower, fill_color);
    Panel_load2led();
}

void Buff_WithCenter(Color_t fill_color, Color_t center_color)
{
    Panel_Clear();
    DrawPatternOverlay(center, center_color);
    DrawPatternOverlay(buff, fill_color);
    Panel_load2led();
}



// static const uint8_t arrow_up[8][8] = {
//     {0,0,0,1,1,0,0,0},
//     {0,0,1,1,1,1,0,0},
//     {0,1,0,1,1,0,1,0},
//     {1,0,0,1,1,0,0,1},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
// };
//
// static const uint8_t arrow_down[8][8] = {
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {0,0,0,1,1,0,0,0},
//     {1,0,0,1,1,0,0,1},
//     {0,1,0,1,1,0,1,0},
//     {0,0,1,1,1,1,0,0},
//     {0,0,0,1,1,0,0,0},
// };
//
// static const uint8_t arrow_left[8][8] = {
//     {0,0,0,1,0,0,0,0},
//     {0,0,1,0,0,0,0,0},
//     {0,1,0,0,0,0,0,0},
//     {1,1,1,1,1,1,1,1},
//     {1,1,1,1,1,1,1,1},
//     {0,1,0,0,0,0,0,0},
//     {0,0,1,0,0,0,0,0},
//     {0,0,0,1,0,0,0,0},
// };
//
// static const uint8_t arrow_right[8][8] = {
//     {0,0,0,0,1,0,0,0},
//     {0,0,0,0,0,1,0,0},
//     {0,0,0,0,0,0,1,0},
//     {1,1,1,1,1,1,1,1},
//     {1,1,1,1,1,1,1,1},
//     {0,0,0,0,0,0,1,0},
//     {0,0,0,0,0,1,0,0},
//     {0,0,0,0,1,0,0,0},
// };
//
// static const uint8_t arrow_cw[8][8] = {
//     {0,0,1,1,1,1,0,0},
//     {0,1,0,0,0,0,1,0},
//     {1,0,0,0,0,0,1,0},
//     {1,0,0,0,0,1,1,1},
//     {1,0,0,0,0,0,1,0},
//     {0,1,0,0,0,0,0,0},
//     {0,0,1,1,1,0,0,0},
//     {0,0,0,0,0,0,0,0},
// };
//
//
// static const uint8_t arrow_ccw[8][8] = {
//     {0,0,1,1,1,1,0,0},
//     {0,1,0,0,0,0,1,0},
//     {0,1,0,0,0,0,0,1},
//     {1,1,1,0,0,0,0,1},
//     {0,1,0,0,0,0,0,1},
//     {0,0,0,0,0,0,1,0},
//     {0,0,0,1,1,1,0,0},
//     {0,0,0,0,0,0,0,0},
// };
/*
*static const uint8_t border_frame[8][8] = {
{1,1,1,1,1,1,1,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{1,0,0,0,0,0,0,1},
{1,1,1,1,1,1,1,1},
};


static const uint8_t needle_up[8][8] = {
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
{0,1,1,1,1,1,0,0},
{0,1,1,1,1,1,0,0},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
};

static const uint8_t needle_down[8][8] = {
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{0,1,1,1,1,1,0,0},
{0,1,1,1,1,1,0,0},
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
{0,0,0,1,1,0,0,0},
};

static const uint8_t needle_left[8][8] = {
{0,0,0,0,0,0,1,1},
{0,0,0,0,1,1,1,1},
{0,0,0,0,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{0,0,0,0,1,1,1,1},
{0,0,0,0,1,1,1,1},
{0,0,0,0,0,0,1,1},
};

static const uint8_t needle_right[8][8] = {
{1,1,0,0,0,0,0,0},
{1,1,1,1,0,0,0,0},
{1,1,1,1,0,0,0,0},
{1,1,1,1,1,1,1,1}, // 细针向右延伸
{1,1,1,1,1,1,1,1},
{1,1,1,1,0,0,0,0},
{1,1,1,1,0,0,0,0},
{1,1,0,0,0,0,0,0},
};


static const uint8_t icon_tower[8][8] = {
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1},
{1,1,1,1,1,1,1,1}
};

// 粗体 X：通过 2x2 的中心交叉点增强远距离视觉焦�
static const uint8_t icon_x[8][8] = {
{1,1,0,0,0,0,1,1},
{1,1,1,0,0,1,1,1},
{0,1,1,1,1,1,1,0},
{0,0,1,1,1,1,0,0}, // 中心加粗
{0,0,1,1,1,1,0,0},
{0,1,1,1,1,1,1,0},
{1,1,1,0,0,1,1,1},
{1,1,0,0,0,0,1,1},
};
*/
