#ifndef FANCY_MOTOR_LIGHTASK_H
#define FANCY_MOTOR_LIGHTASK_H
#include <stdint.h>
#include "target.h"
void buffTask_loop(uint32_t now);
void buffTask_Init(void);
void Game_UpdateStripEffects(void);

extern uint8_t reset_flag;

#endif