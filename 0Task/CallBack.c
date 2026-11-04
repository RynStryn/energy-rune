//
// Created by 31466 on 2026/2/6.
//

#include "buffTask.h"
#include "stm32h7xx_hal.h"
#include "motorTask.h"
#include "WS2812B.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 判断是哪个定时器触发的
    if (htim->Instance == TIM6) {
        Rune_Motor_Loop();
        buffTask_loop();
    }
}
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{

    if (htim->Instance == TIM3) {

        // 为了保险，把该定时器涉及的所有通道都 Stop
        // 关键：必须调用 Stop_DMA，否则 HAL 状态机认为还在 Busy，下次发不出去
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_3);

    }

    if (htim->Instance == TIM4) {
        HAL_TIM_PWM_Stop_DMA(htim, TIM_CHANNEL_2);
    }
}