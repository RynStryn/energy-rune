#include "led.h"
#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////
// LED驱动代码与按键检测逻辑
//////////////////////////////////////////////////////////////////////////////////

// 引用 control_task.c 定义的全局变量
extern int game_stage;
extern int bonus_window_timer;
extern uint8_t flag_first_hit;
extern leaf LED[5];  // 确保能访问LED数组

// 原有全局变量保留
int Flag_Input_Circle4;
int Flag_Input_Circle3;
int Flag_Input_Circle2;
int Flag_Input_Circle1;

int Flag_Input_1 = 0;
int Flag_Input_2 = 0;
int Flag_Input_3 = 0;
int Flag_Input_4 = 0;
int Flag_Input_5 = 0;
int Flag_Input_n = 0;
int Flag_Input_1_Last = 0;
int Flag_Input_2_Last = 0;
int Flag_Input_3_Last = 0;
int Flag_Input_4_Last = 0;
int Flag_Input_5_Last = 0;

int flag2_return = 0;
int flag3_return = 0;
int flag4_return = 0;
int flag5_return = 0;

int Flag_Input_PA7;
int Flag_Input_PA6;
int Flag_Input_PC4;
int Flag_Input_PA7_Last;
int Flag_Input_PA6_Last;
int Flag_Input_PC4_Last;

int LED_mode[5];
int LED_circle_mode[5];
int Leaf_mode[5];

int cnt_Circle_1;
int cnt_Circle_2;
int cnt_Circle_3;
int cnt_Circle_4;
uint16_t count_reset = 0;
int flag_rc1;
int flag_rc11;
uint16_t reset_time = 0;

/**
 * @brief 核心裁判逻辑：处理击打、误打、连击及进度
@note 压感器input、遥控器调试
 */
void key_input(void) {
  // ================= 1. 读取传感器状态 =================
  Flag_Input_1 = (Sucker_PB_5);   // LED[0]
  Flag_Input_2 = (Sucker_PB_4);   // LED[1] (CAN接收或本地读取)
  Flag_Input_3 = (Sucker_PB_3);   // LED[2] (CAN接收或本地读取)
  Flag_Input_4 = (Sucker_PC_12);  // LED[3]
  Flag_Input_5 = (Sucker_PC_11);  // LED[4]
  Flag_Input_n = (Sucker_PA_1);   // 调试/遥控按钮

  // 遥控过符逻辑 (保留原有逻辑)
  if (Flag_Input_n == 1)
    flag_rc1 = 1;
  if (Flag_Input_n == 0 && flag_rc1 == 1)
    flag_rc11 = 1;

  // 整理为数组便于循环处理
  int Sensors_Current[5] = {Flag_Input_1, Flag_Input_2, Flag_Input_3,
                            Flag_Input_4, Flag_Input_5};
  int Sensors_Last[5] = {Flag_Input_1_Last, Flag_Input_2_Last,
                         Flag_Input_3_Last, Flag_Input_4_Last,
                         Flag_Input_5_Last};

  // ================= 2. 核心裁判逻辑 (循环判定) =================
  for (int i = 0; i < 5; i++) {
    // 模拟遥控击打 (如果处于等待状态且触发了遥控按钮)
    if (LED[i].mode == Waiting_hit && flag_rc11 == 1) {
      Sensors_Current[i] = 1;  // 强制模拟上升沿
      if (i == 4) {
        flag_rc1 = 0;
        flag_rc11 = 0;
      }  // 最后一个处理完后重置遥控标号
    }

    // 上升沿检测 (由 0 变 1)
    if (Sensors_Last[i] == 0 && Sensors_Current[i] == 1) {
      if (LED[i].mode == Waiting_hit) {
        // --- [A] 有效击打 ---
        LED[i].mode = hit_finish;
        count_reset = 0;  // 击中即重置超时计数

        if (flag_first_hit == 0) {
          // 本轮首击成功
          flag_first_hit = 1;
          game_stage++;            // 进度增加
          bonus_window_timer = 0;  // 开启连击计时窗口
        } else {
          // 本轮连击成功 (击中第二个目标)
          // 设置计时器达到阈值，通知 control_task 结束本轮并进入下一轮
          bonus_window_timer = 1000;
        }
      } else if (LED[i].mode == LED_none) {
        // --- [B] 误击惩罚 (击中灭掉的扇叶) ---
        game_stage = 0;      // 进度清零
        count_reset = 4000;  // 强制触发超时重置效果
      }
    }
  }

  // 遥控清理（确保循环外重置）
  if (flag_rc11 == 1) {
    flag_rc1 = 0;
    flag_rc11 = 0;
  }

  // ================= 3. 超时/手动重置判定 =================

  // 只有在未完成首击时才进行常规超时计算 (2.5秒示例)
  if (flag_first_hit == 0 && game_stage < 5) {
    int any_active = 0;
    for (int k = 0; k < 5; k++)
      if (LED[k].mode == Waiting_hit)
        any_active = 1;

    if (any_active) {
      count_reset++;
      
if (count_reset >= 2500)  // 2.5s 超时
      {
        game_stage = 0;  // 失败重置进度
        for (int k = 0; k < 5; k++)
          LED[k].mode = LED_none;
        count_reset = 0;
      }

    }
  }

  // 原有手动重置逻辑
  if (ready_to_reset == 1 && RC_CtrlData.rc.s2 == 3)
    count_reset = 20000;

  if (count_reset >= 4000) {
    for (int k = 0; k < 5; k++)
      LED[k].mode = LED_none;
    count_reset = 0;
  }

  // ================= 4. 更新状态历史 =================
  Flag_Input_1_Last = Flag_Input_1;
  Flag_Input_2_Last = Flag_Input_2;
  Flag_Input_3_Last = Flag_Input_3;
  Flag_Input_4_Last = Flag_Input_4;
  Flag_Input_5_Last = Flag_Input_5;
}

// ================= 底层初始化函数 (保持原样) =================

void LED_Init(void) {
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2);
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
