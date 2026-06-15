//
// Created by RynStryn on 2026/5/12.
//

#ifndef TC_UU_D_MKLK_COLOR_H
#define TC_UU_D_MKLK_COLOR_H

#include <stdint.h>

/* ============================================================
 *  颜色值类型 —— RGB 三通道
 * ============================================================
 *  使用结构体传递颜色，替代分散的 (r,g,b) 参数。
 *  在 ARM Cortex-M3 上，3 字节结构体通过寄存器传递，
 *  不会比三个分散的 uint8_t 参数产生额外开销。
 * ============================================================ */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color_t;

/* ---- 常用颜色的工厂宏（在调用处构造临时 Color_t） ---- */
#define COLOR_RGB(r_, g_, b_)  ((Color_t){(r_), (g_), (b_)})

/* ---- 预定义颜色常量 (static const 可以内联到调用处) ---- */
static const Color_t COLOR_BLACK   = {  0,   0,   0};
static const Color_t COLOR_RED     = {200,   0,   0};
static const Color_t COLOR_GREEN   = {  0, 200,   0};
static const Color_t COLOR_BLUE    = {  0,   0, 200};
static const Color_t COLOR_YELLOW  = {255, 255,   0};
static const Color_t COLOR_CYAN    = {  0, 255, 255};
static const Color_t COLOR_MAGENTA = {255,   0, 255};
static const Color_t COLOR_WHITE   = {255, 255, 255};
static const Color_t COLOR_ORANGE  = {255, 165,   0};
static const Color_t COLOR_PURPLE  = {128,   0, 128};
static const Color_t COLOR_LIME    = {  0, 255,   0};

#endif // TC_UU_D_MKLK_COLOR_H
