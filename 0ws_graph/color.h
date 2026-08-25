//
// Created by RynStryn on 2026/5/12.
//

#ifndef TC_UU_D_MKLK_COLOR_H
#define TC_UU_D_MKLK_COLOR_H

#include <stdint.h>


typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color_t;


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
