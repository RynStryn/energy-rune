#ifndef __LED_H
#define __LED_H
#include <stdint.h>

#include "stm32h7xx_hal.h"
extern uint8_t strip_mode[5];
//LED端口定义
#define LED0_ON  GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define LED1_ON  GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define LED0_OFF GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define LED1_OFF GPIO_SetBits(GPIOC,GPIO_Pin_2)

//
#define Sucker_PB_5   PBin(5)
#define Sucker_PB_4   PBin(4)
#define Sucker_PB_3  PBin(3)
#define Sucker_PC_12    PCin(12)
#define Sucker_PC_11  PCin(11)

void hitDetect(void);
void circle_key_input(uint16_t leaf);
void Energy_Input(uint16_t leaf);

void Sucker_PC12_Init(void);
void Sucker_PC11_Init(void);
void Sucker_PB5_Init(void);
void Sucker_PB4_Init(void);
void Sucker_PB3_Init(void);



void LED_Init(void);//初始化
#endif
