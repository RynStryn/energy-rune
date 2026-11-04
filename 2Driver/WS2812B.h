#ifndef _WS2812B_H
#define _WS2812B_H
#include "stdint.h"
//64 184 111
//40 30 20 12 6 3
//定义灯珠数量 多发一个灯的 保证数据稳定
#define NEW_PLUS 35
#define MY_WS2812_MAX_NUM  (64*5+184+111+35+10+13+35)
#define SIDE_START (64*5)
#define CIRCLE_START (64*5+184)


//#define MY_WS2812_MAX_NUM  64+184+111
//#define SIDE_START 64
//#define CIRCLE_START 64+184
#define WS28_SENDBUFF_SIZE	        (MY_WS2812_MAX_NUM*24)//16,992
#define CIRCLE6_START CIRCLE_START
#define CIRCLE5_START (CIRCLE_START+40)
#define CIRCLE4_START (CIRCLE_START+40+30)
#define CIRCLE3_START (CIRCLE_START+40+30+20)
#define CIRCLE2_START (CIRCLE_START+40+30+20+12)
#define CIRCLE1_START (CIRCLE_START+40+30+20+12+6)
#define CIRCLE2_RESTART (CIRCLE_START+40+30+20+12+6+3)
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t           Channel;
} WS2812_Config_t;
typedef enum {
    energe_leaf1 = 0,
    energe_leaf2,
    energe_leaf3,
    energe_leaf4,
    energe_leaf5
} energe_leaf_type;


typedef enum {
    Energe_none = 0,
    Small_energe,
    Big_energe
} energe_mode_type;

typedef enum {
    LED_none = 0,
    Waiting_hit,
    hit_finish,
    all_finish
} LED_mode_type;

typedef enum {
    LED_circle_none = 0,
    LED_circle1,
    LED_circle2,
    LED_circle3
} LED_circle_mode_type;

typedef enum {
    LED_leaf_none = 0,
    LED_leaf1,
    LED_leaf2,
    LED_leaf3,
    LED_leaf4,
    LED_leaf5
} Leaf_mode_type;


//低电平偏移，复位
//似乎没用上?
#define MY_WS2812_RST_NUM  70000//RESET >=280 miu s ,600->70000


//f4的版本:
// tim.TIM_Period        = 13 - 1;
// tim.TIM_Prescaler     = 8 - 1;
#define TIMING_ONE  180//19?->434?//溢出!
#define TIMING_ZERO 72//3->69


/****************** 函 数 声 明 *****************************/


void LED_TargetMode(uint16_t leaf);

void my_ws2812_1_set(uint16_t leaf,
                     uint16_t num,
                     uint8_t  rv,
                     uint8_t  gv,
                     uint8_t  bv);

void my_ws2812_set_all_off(uint16_t leaf);

void my_ws2812_set_all_on(uint16_t leaf);

void LED_current_mode(uint16_t leaf, int mode, int line);

void set_side_circle_on(uint16_t leaf);

void side_off(uint16_t leaf);

void side_on(uint16_t leaf);

void LED_current(uint16_t leaf);

void LED_current_all_on(uint16_t leaf);

void LED_current_all_off(uint16_t leaf);

void LED_circle(uint16_t leaf);

void LED_target(uint16_t leaf);

void LED_target_allOff(uint16_t leaf);

void LED_currentAll(uint16_t leaf);

void LED_Strip(uint16_t leaf);


void transfer(uint8_t leaf);

/***************************************************************/

#endif
