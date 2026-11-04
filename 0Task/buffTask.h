//
// Created by 31466 on 2026/2/1.
//

#ifndef FANCY_MOTOR_LIGHTASK_H
#define FANCY_MOTOR_LIGHTASK_H
#include <stdint.h>
#include "led.h"


typedef enum {
    STATE_GENERATE,    // 生成
    STATE_WAITING,     // 等待击打
    STATE_BONUS_WINDOW // 连击
} eBigRuneState;

void rand_energe_leaf(void);
void buffTask_loop(void);
void clean_time(void);
void buffTask_Init(void);



extern int time_tick;
extern int all_finish_time[5];
extern int animationFrame ;
extern int last_led_time1 ;
extern int energe_mode ;
extern int energe_leaf ;
extern long long time_Big_energe;

extern uint8_t n;
extern uint8_t flag_start;
extern uint8_t ready_to_reset;
#endif //FANCY_MOTOR_LIGHTASK_H