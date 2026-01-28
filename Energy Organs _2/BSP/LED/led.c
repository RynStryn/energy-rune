#include "led.h"
#include "sys.h"

/********************************************************************************/
// 副板传感器输入处理
// 逻辑：检测传感器上升沿 -> 改变本地灯效模式 -> 产生信号供 control_task
// 通过CAN发送给主板
/********************************************************************************/

int Flag_Input_1 = 0;
int Flag_Input_2 = 0;
int Flag_Input_3 = 0;
int Flag_Input_4 = 0;
int Flag_Input_5 = 0;

int Flag_Input_1_Last = 0;
int Flag_Input_2_Last = 0;
int Flag_Input_3_Last = 0;
int Flag_Input_4_Last = 0;
int Flag_Input_5_Last = 0;

int LED_mode[5];
int LED_circle_mode[5];
int Leaf_mode[5];

// 传感器读取与命中检测逻辑
void key_input(void) {
  /* 1. 读取传感器电平状态 (假设副板控制的是 LED[1] 和 LED[2]) */
  if (LED[1].mode == Waiting_hit) {
    Flag_Input_2 = (!Sucker_PB_4);  // 读取2号叶片传感器
  }

  if (LED[2].mode == Waiting_hit) {
    Flag_Input_3 = (!Sucker_PB_3);  // 读取3号叶片传感器
  }

  /* 2. 命中判定（上升沿检测） */
  // 处理 2 号叶片
  if (LED[1].mode == Waiting_hit && Flag_Input_2_Last == 0 &&
      Flag_Input_2 == 1) {
    LED[1].mode = hit_finish;
    // 提示：此处改变 mode 后，control_task 中的 CAN 发送逻辑应检测到 mode
    // 改变并通知主板
  }

  // 处理 3 号叶片
  if (LED[2].mode == Waiting_hit && Flag_Input_3_Last == 0 &&
      Flag_Input_3 == 1) {
    LED[2].mode = hit_finish;
  }

  /* 3. 更新历史状态 */
  Flag_Input_2_Last = Flag_Input_2;
  Flag_Input_3_Last = Flag_Input_3;
}

// LED IO 初始化
void LED_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2);  // 默认熄灭
}

void Sucker_PC11_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruecture);
  GPIO_ResetBits(GPIOC, GPIO_Pin_11);
}

void Sucker_PC12_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruecture);
  GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}

void Sucker_PB5_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruecture);
  GPIO_ResetBits(GPIOB, GPIO_Pin_5);
}

void Sucker_PA1_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruecture);
}

void Sucker_PB4_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruecture);
  GPIO_ResetBits(GPIOB, GPIO_Pin_4);
}

void Sucker_PB3_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruecture;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  GPIO_InitStruecture.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruecture.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStruecture.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruecture.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruecture.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruecture);
  GPIO_ResetBits(GPIOB, GPIO_Pin_3);
}