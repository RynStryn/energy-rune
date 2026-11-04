#include "stm32h7xx_hal.h"
#include "ws2812b.h"
#include <string.h>
#include "buffTask.h"
#include "DMA_buffer_config.h"

extern int game_stage;

int current_r = 150;
int side_r    = 254;

int r_brt = 250; //brightness
int g_brt = 0;
int b_brt = 0;

int r_off_brt = 0;
int g_off_brt = 0;
int b_off_brt = 0;

int r_edg_brt = 2;
int g_edg_brt = 0;
int b_edg_brt = 0;

int cnt = 0;

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

// ================== 1. 纯硬件配置  ==================

static const WS2812_Config_t Hw_Map[5] = {
    {&htim3, TIM_CHANNEL_1},
    // Leaf 0
    {&htim3, TIM_CHANNEL_2},
    // Leaf 1
    {&htim3, TIM_CHANNEL_3},
    // Leaf 2
    {&htim4, TIM_CHANNEL_1},
    // Leaf 3
    {&htim4, TIM_CHANNEL_2} // Leaf 4
};


void transfer(uint8_t leaf)
{
    if (leaf >= 5) return;
    const WS2812_Config_t *hw = &Hw_Map[leaf];

    if (leaf >= 0 || leaf <= 2)
        HAL_TIM_PWM_Start_DMA(hw->htim,
                              hw->Channel,
                              (uint32_t *) RGB_DMA_buffer4TIM3[leaf],
                              WS28_SENDBUFF_SIZE);
    if (leaf == 3 || leaf == 4)
        HAL_TIM_PWM_Start_DMA(hw->htim,
                              hw->Channel,
                              (uint32_t *) RGB_DMA_buffer4TIM4[leaf - 3],
                              WS28_SENDBUFF_SIZE);
}

void my_ws2812_1_set(uint16_t leaf,
                     uint16_t num,
                     uint8_t  rv,
                     uint8_t  gv,
                     uint8_t  bv)
{
    if (leaf >= 5) return;

    // 计算偏移量 (每个灯珠 24 bit)
    uint32_t  indexx = ((num - 1) * 24);
    uint16_t *pBuff  = (leaf <= 2)
                          ? RGB_DMA_buffer4TIM3[leaf]
                          : RGB_DMA_buffer4TIM4[leaf - 3];
    for (uint8_t i = 0; i < 8; i++)
    {
        pBuff[indexx + i] = ((gv << i) & 0x80)
                                ? TIMING_ONE
                                : TIMING_ZERO;
        pBuff[indexx + i + 8] = ((rv << i) & 0x80)
                                    ? TIMING_ONE
                                    : TIMING_ZERO;
        pBuff[indexx + i + 16] = ((bv << i) & 0x80)
                                     ? TIMING_ONE
                                     : TIMING_ZERO;
    }
}

// 关闭所有
void my_ws2812_set_all_off(uint16_t leaf)
{
    for (uint16_t i = 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
}

// 开启所有
void my_ws2812_set_all_on(uint16_t leaf)
{
    for (uint16_t i = 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, r_brt, g_brt, b_brt);
    }
}


void LED_current_mode(uint16_t leaf, int mode, int line)
{
    switch (mode)
    {
        case 1:
            my_ws2812_1_set(leaf, line * 8 + 1, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, 0, 0, 0);
            break;
        case 2:
            my_ws2812_1_set(leaf, line * 8 + 1, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, 0, 0, 0);
            break;
        case 3:
            my_ws2812_1_set(leaf, line * 8 + 1, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, 0, 0, 0);
            break;
        case 4:
            my_ws2812_1_set(leaf, line * 8 + 1, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, 0, 0, 0);
            break;
        case 5:
            my_ws2812_1_set(leaf, line * 8 + 1, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, 0, 0, 0);
            break;
        case 6:
            my_ws2812_1_set(leaf, line * 8 + 1, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, current_r, 0, 0);
            break;
        case 7:
            my_ws2812_1_set(leaf, line * 8 + 1, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, current_r, 0, 0);
            break;
        case 8:
            my_ws2812_1_set(leaf, line * 8 + 1, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 2, current_r, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 3, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 4, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 5, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 6, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 7, 0, 0, 0);
            my_ws2812_1_set(leaf, line * 8 + 8, current_r, 0, 0);
            break;
        default: ;
    }
}


void LED_Strip(uint16_t leaf)
{
    // int mode;

    // j 代表灯臂的 5 个段 (0-4)
    for (int j = 0; j < 5; j++)
    {
        // 【关键逻辑】只有在当前激活进度内的段才显示
        // game_stage = 1 时，显示 j=0 的段 (1/5)
        // game_stage = 2 时，显示 j=0,1 的段 (2/5)
        if (j < game_stage)
        {
            for (int k = 0; k < 8; k++)
            {
                int line = j * 8 + k;
                for (int p = 1; p <= 8; p++)
                {
                    my_ws2812_1_set(leaf, line * 8 + p, current_r, 0, 0);
                }
            }
        } else
        {
            // --- 未激活区域：强制熄灭 ---
            // 每个 j 段包含 8 行 (line)，每行 8 颗灯珠
            // 段 j 的行范围是: [j*8] 到 [j*8 + 7]
            for (int k = 0; k < 8; k++)
            {
                int line = j * 8 + k;
                // 熄灭该行对应的 8 颗灯珠 (索引 line*8+1 到 line*8+8)
                for (int p = 1; p <= 8; p++)
                {
                    my_ws2812_1_set(leaf, line * 8 + p, 0, 0, 0);
                }
            }
        }
    }
}

// 已经被strip替换
void LED_current(uint16_t leaf)
{
    int mode;

    // j 代表灯臂的 5 个段 (0-4)
    for (int j = 0; j < 5; j++)
    {
        // 【关键逻辑】只有在当前激活进度内的段才显示流水灯
        // game_stage = 1 时，显示 j=0 的段 (1/5)
        // game_stage = 2 时，显示 j=0,1 的段 (2/5)
        if (j < game_stage)
        {
            // --- 激活区域：正常播放流水灯特效 ---
            for (int i = 0; i < 4; i++)
            {
                mode = animationFrame - i;
                while (mode < 1)
                    mode += 8;
                // 计算行号并显示
                LED_current_mode(leaf, mode, 3 - i + j * 8);
                LED_current_mode(leaf, mode, 4 + i + j * 8);
            }
        } else
        {
            // --- 未激活区域：强制熄灭 ---
            // 每个 j 段包含 8 行 (line)，每行 8 颗灯珠
            // 段 j 的行范围是: [j*8] 到 [j*8 + 7]
            for (int k = 0; k < 8; k++)
            {
                int line = j * 8 + k;
                // 熄灭该行对应的 8 颗灯珠 (索引 line*8+1 到 line*8+8)
                for (int p = 1; p <= 8; p++)
                {
                    my_ws2812_1_set(leaf, line * 8 + p, 0, 0, 0);
                }
            }
        }
    }
}

void LED_current_all_on(uint16_t leaf)
{
    for (int i = 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
}

void LED_current_all_off(uint16_t leaf)
{
    for (int i = 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
}

void LED_circle(uint16_t leaf)
{
    for (int i = CIRCLE6_START + 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
}

void LED_target(uint16_t leaf)
{
    for (int i = CIRCLE6_START + 1; i <= CIRCLE5_START; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
    for (int i = CIRCLE5_START + 1; i <= CIRCLE4_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE4_START + 1; i <= CIRCLE3_START; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
    for (int i = CIRCLE3_START + 1; i <= CIRCLE2_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE2_START + 1; i <= CIRCLE1_START - 1; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
    for (int i = CIRCLE1_START; i <= CIRCLE2_RESTART; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE2_RESTART + 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 254, 0, 0);
    }
}

// 1.28

void LED_target_allOff(uint16_t leaf)
{
    for (int i = CIRCLE6_START + 1; i <= CIRCLE5_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE5_START + 1; i <= CIRCLE4_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE4_START + 1; i <= CIRCLE3_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE3_START + 1; i <= CIRCLE2_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE2_START + 1; i <= CIRCLE1_START - 1; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE1_START; i <= CIRCLE2_RESTART; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
    for (int i = CIRCLE2_RESTART + 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
}

void LED_currentAll(uint16_t leaf)
{
    int mode;

    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            mode = animationFrame - i;

            while (mode < 1)

                mode += 8;

            LED_current_mode(leaf, mode, 3 - i + j * 8);

            LED_current_mode(leaf, mode, 4 + i + j * 8);
        }
    }
}
#if 0
// 这个函数只管“踢一脚”DMA
static void DMA_Kickoff(const WS2812_Config_t *hw, uint32_t data_count)
{
    DMA_ClearFlag(hw->dma_stream, hw->dma_tc_flag);
    DMA_SetCurrDataCounter(hw->dma_stream, data_count);
    DMA_Cmd(hw->dma_stream, ENABLE);

    TIM_CCxCmd(hw->timer, hw->timer_channel, TIM_CCx_Enable);
    TIM_Cmd(hw->timer, ENABLE);

    while (DMA_GetFlagStatus(hw->dma_stream, hw->dma_tc_flag) == RESET);

    TIM_CCxCmd(hw->timer, hw->timer_channel, TIM_CCx_Disable);
    TIM_Cmd(hw->timer, DISABLE);
    DMA_Cmd(hw->dma_stream, DISABLE);
    DMA_ClearFlag(hw->dma_stream, hw->dma_tc_flag);
}

int Flag_Input_Circle3;

int Flag_Input_Circle1;

int LED_circle_mode[5];

int  Flag_Input_Circle2;
void LED_TargetMode(uint16_t leaf)
{
    if (Flag_Input_Circle3 == 0 && Flag_Input_Circle2 == 0 &&
        Flag_Input_Circle1 == 0)
        LED_circle_mode[leaf] = 0; // 0表示全灭
    else if (Flag_Input_Circle3 == 0 && Flag_Input_Circle2 == 0 &&
             Flag_Input_Circle1 == 1)
        LED_circle_mode[leaf] = 1; // 1表示环数1亮
    else if (Flag_Input_Circle3 == 0 && Flag_Input_Circle2 == 1 &&
             Flag_Input_Circle1 == 0)
        LED_circle_mode[leaf] = 2; // 2表示环数2亮
    else if (Flag_Input_Circle3 == 1 && Flag_Input_Circle2 == 0 &&
             Flag_Input_Circle1 == 0)
        LED_circle_mode[leaf] = 3; // 3表示环数3亮
    else if (Flag_Input_Circle3 == 0 && Flag_Input_Circle2 == 1 &&
             Flag_Input_Circle1 == 1)
        LED_circle_mode[leaf] = 4; // 4表示环数1、2亮
    else if (Flag_Input_Circle3 == 1 && Flag_Input_Circle2 == 0 &&
             Flag_Input_Circle1 == 1)
        LED_circle_mode[leaf] = 5; // 5表示环数1、3亮
    else if (Flag_Input_Circle3 == 1 && Flag_Input_Circle2 == 1 &&
             Flag_Input_Circle1 == 0)
        LED_circle_mode[leaf] = 6; // 6表示环数2、3亮
    else if (Flag_Input_Circle3 == 1 && Flag_Input_Circle2 == 1 &&
             Flag_Input_Circle1 == 1)
        LED_circle_mode[leaf] = 7; // 7表示环数1、2、3亮
}

void Circle3_Open(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 1, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 2, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 3, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 4, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 5, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 6, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 7, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 8, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 9, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 10, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 11, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 12, 222, 0, 0);
}
void Circle2_Open(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 13, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 14, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 15, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 16, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 17, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 18, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 19, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 20, 222, 0, 0);
}
void Circle1_Open(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 21, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 22, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 23, 222, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 24, 222, 0, 0);
}
void Circle3_Close(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 1, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 2, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 3, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 4, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 5, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 6, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 7, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 8, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 9, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 10, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 11, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 12, 0, 0, 0);
}
void Circle2_Close(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 13, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 14, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 15, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 16, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 17, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 18, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 19, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 20, 0, 0, 0);
}
void Circle1_Close(uint16_t leaf)
{
    my_ws2812_1_set(leaf, CIRCLE_START + 21, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 22, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 23, 0, 0, 0);
    my_ws2812_1_set(leaf, CIRCLE_START + 24, 0, 0, 0);
}
void set_side_circle_on(uint16_t leaf)
{
    for (uint16_t i = SIDE_START + 1; i <= MY_WS2812_MAX_NUM; i++)
    {
        my_ws2812_1_set(leaf, i, 222, 0, 0);
    }
}

void side_off(uint16_t leaf)
{
    for (uint16_t i = SIDE_START + 1; i <= CIRCLE_START; i++)
    {
        my_ws2812_1_set(leaf, i, 0, 0, 0);
    }
}

void side_on(uint16_t leaf)
{
    for (uint16_t i = SIDE_START + 1; i <= CIRCLE_START; i++)
    {
        my_ws2812_1_set(leaf, i, side_r, 0, 0);
    }
}
#endif
