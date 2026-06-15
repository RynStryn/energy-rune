//
// Created by 31466 on 2026/2/5.
//

#include "motorTask.h"
#include <stdlib.h>
#include "can.h"
#include "pid.h"
#include "software_config.h"
#include "stm32h7xx_hal.h"
#include "target.h"
// =================
#define PI 3.1415926f
#define IDLE_SPEED_RAD (PI / 3.0f)
#define REDUCTION_RATIO 19.0f
#define RAD_TO_ANGLE (180.0f / PI)

#define SPEED_FACTOR (RAD_TO_ANGLE * REDUCTION_RATIO / 6.0f)

// ================= 全局变量 =================
Motor_App_t motor_app =
    {.a = 0.780f, .b = 1.310f, .w = 1.884f, .t = 0, .rand_a = 0, .rand_w = 0};
// uint32_t motor_time     = 0;
// uint32_t random_val;
uint8_t is_rune_active = 1;

Motor_Encoder_t MotorC620_Encoder;
Motor_Encoder_t motor_encoder;
pid_t pid;
pid_t pid_big;
FDCAN_TxHeaderTypeDef MotorC620_TxHeader;

extern FDCAN_HandleTypeDef hfdcan1;
extern RNG_HandleTypeDef hrng;
// ================= 核心函数 =================

/**
 * @brief 参数重置函数：重新随机生成 a, omega, b, 并重置 t=0
 * 触发时机：1.开机 2.All Finish展示结束
 * @note rand() 是伪随机，由seed唯一决定;
 * srand(x) 生成新的种子
 */

void Motor_ParamsReset(void) {
  if (HAL_RNG_GenerateRandomNumber(&hrng, &motor_app.rand_a) != HAL_OK) {
    gError.RNGwrong++;
    motor_app.rand_a = 0x7FFFFFFF;
  }
  if (HAL_RNG_GenerateRandomNumber(&hrng, &motor_app.rand_w) != HAL_OK) {
    gError.RNGwrong++;
    motor_app.rand_w = 0x7FFFFFFF;
  }
  motor_app.t = 0;
#if 0
    srand(motor_time);
    //先用之前的time做种,否则每次都一样
    motor_time = 0;
#endif
  // 随机生成 a: [0.780, 1.045]
  // rand() 返回 0~RAND_MAX
  float rand_factor_a = (float)motor_app.rand_a / 4294967295.0f;
  motor_app.a = 0.780f + rand_factor_a * (1.045f - 0.780f);

  // 随机生成 omega: [1.884, 2.000]
  float rand_factor_w = (float)motor_app.rand_w / 4294967295.0f;
  motor_app.w = 1.884f + rand_factor_w * (2.000f - 1.884f);
  motor_app.b = 2.090f - motor_app.b;
}

void Rune_Motor_Loop(void) {
  float speed_rad = 0.0f;
  if (g_game.rune_mode == SMALL_RUNE) {
    speed_rad = PI / 3.0f;
  } else {
    // ( 如果Loop 1ms 调用一次)
    float t = (float)motor_app.t / 1000.0f;
    speed_rad = motor_app.a * sinf(motor_app.w * t) + motor_app.b;
    motor_app.t++;
  }

  // 注意 rotate_direction
  float target_val = speed_rad * SPEED_FACTOR;

  pid_calc(&pid, MotorC620_Encoder.rotate_rate, target_val);
  can_sent(&hfdcan1, &MotorC620_TxHeader, pid.out, 0, 0, 0);
}

void motor_run_init(void) {
  PID_struct_init(&pid, POSITION_PID, 25000, 8000, 15, 0.5, 0);  // 双环值
}
