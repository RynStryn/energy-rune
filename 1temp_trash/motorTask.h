//
// Created by 31466 on 2026/2/5.
//

#ifndef FANCY_MOTOR_MOTORTASK_H
#define FANCY_MOTOR_MOTORTASK_H
#include <stdint.h>


typedef struct {
    int32_t  raw_value;
    int32_t  last_raw_value;
    int32_t  diff;
    int32_t  round_cnt;
    double   ecd_angle;
    int32_t  ecd_value;
    int32_t  rate_buf[6];
    int16_t  filter_rate;
    int16_t  rotate_rate;
    int32_t  angle_rate;
    uint32_t temperature;
    int32_t  ecd_current;
    double   torque_current;
    double   torque;

    int32_t temp_count;
    uint8_t buf_count;
    int32_t ecd_bias;
    double  angle_bias;
    int32_t ecd_raw_rate;
} Motor_Encoder_t;

extern Motor_Encoder_t motor_encoder;

typedef struct {
    float    a;
    float    b;
    float    w;
    uint32_t t;
    uint32_t rand_a;
    uint32_t rand_w;
} Motor_App_t;

extern Motor_App_t motor_app;

void Rune_Motor_Loop(void);

void Motor_ParamsReset(void);
#endif //FANCY_MOTOR_MOTORTASK_H
