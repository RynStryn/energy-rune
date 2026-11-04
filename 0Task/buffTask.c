#include "buffTask.h"
#include <stdlib.h>
#include "target_driver.h"
#include "WS2812B.h"
#include "led.h"
#include "software_config.h"
eBigRuneState BigRuneState = STATE_GENERATE;
// ================= GLOBAL VARS =================
int        flag_punish    = 0;
extern int rune_mode; // 默认大符
int        game_stage     = 0;
uint8_t    flag_first_hit = 0; // 大符首击
uint8_t    flag_start     = 0; // 游戏开始
uint8_t    rand_finish    = 0;
uint32_t   rand1          = 0;
uint32_t   rand2          = 0;
uint32_t   randSmall      = 0;
// 定时器变量
int      time_tick          = 0;
int      bonus_window_timer = 0; // 连击窗口计时
int      activation_timer   = 0; // 2.5s 激活超时计时
uint16_t success_cntdown    = 0; // 全中展示计时


uint8_t strip_mode[5]  = {0};
uint8_t target_mode[5] = {0};

int                      animationFrame = 1; // 动画帧
extern RNG_HandleTypeDef hrng;
// =================  HELPER =================


void LeafReset(void)
{
    for (int i = 0; i < 5; i++)
    {
        strip_mode[i]      = LED_none;
        target_mode[i]     = LED_none;
        game_stage         = 0;
        success_cntdown    = 0;
        flag_first_hit     = 0;
        bonus_window_timer = 0;
        BigRuneState       = 0;
    }
}

// ================= SMALL_RUNE =================
void SmallRuneLoop(void)
{
    if (game_stage >= 5)
    {
        success_cntdown++;
        if (success_cntdown > successShowTime)
        {
            success_cntdown = 0;
            LeafReset();
        }
        for (int m = 0; m < 5; m++)
        {
            if (strip_mode[m] != all_finish)
                strip_mode[m] = all_finish;
            target_mode[m] = 2;
        }
        return;
    }

    int active_index = -1;
    for (int i = 0; i < 5; i++)
    {
        if (strip_mode[i] == Waiting_hit || strip_mode[i] == hit_finish)
        {
            active_index = i;
            break;
        }
    }


    // CASE A: 场上无目标 -> 生成新目标
    if (active_index == -1)
    {
        if (HAL_RNG_GenerateRandomNumber(&hrng, &randSmall) != HAL_OK)
        {
            gError.RNGwrong++;
            randSmall = 0x7FFFFFFF;
        }
        int t1   = randSmall % 5;
        int safe = 0;
        while (strip_mode[t1] == all_finish && safe < generationTimeLimit)
        {
            t1 = randSmall % 5;
            safe++;
        }

        // 确认该扇叶可用
        if (strip_mode[t1] != all_finish)
        {
            strip_mode[t1]   = Waiting_hit;
            activation_timer = 0; // 重置超时计时
        }
    }
    // CASE B: 场上有目标 -> 监控击打或超时
    else
    {
        // --- 判定 1: 击打成功 ---
        if (strip_mode[active_index] == hit_finish)
        {
            game_stage++; // 进度+1

            strip_mode[active_index] = all_finish;

            activation_timer = 0;
        }
        // --- 判定 2: 超时检测 ---
        else
        {
            activation_timer++;
            if (activation_timer > TIME_OUT)
            {
                LeafReset();
            }
        }
    }
}

// ================= BIG_RUNE =================

void BigRuneLoop(void)
{
    if (game_stage >= 5)
    {
        success_cntdown++;
        if (success_cntdown > successShowTime)
        {
            success_cntdown = 0;
            LeafReset();
        }
        for (int m = 0; m < 5; m++)
        {
            strip_mode[m]  = all_finish;
            target_mode[m] = 2;
        }
        return;
    }

    switch (BigRuneState)
    {
        case STATE_GENERATE: {
            if (HAL_RNG_GenerateRandomNumber(&hrng, &rand1) != HAL_OK)
            {
                gError.RNGwrong++;
                rand1 = 0x7FFFFFFF;
            }
            if (HAL_RNG_GenerateRandomNumber(&hrng, &rand2) != HAL_OK)
            {
                gError.RNGwrong++;
                rand2 = 0x7FFFFFFF;
            }
            // 随机生成2个不重复的目标
            int t1 = rand1 % 5;
            int t2 = rand2 % 5;
            // 简单防止死循环的随机
            int safe = 0;
            while (t1 == t2 && safe < generationTimeLimit)
            {
                if (safe > 0.9 * generationTimeLimit)gError.GenerationLoop++;
                if (HAL_RNG_GenerateRandomNumber(&hrng, &rand2) != HAL_OK)
                {
                    gError.RNGwrong++;
                    t2 = (t1 + 1) % 5;
                }
                t2 = rand2 % 5;
                safe++;
            }
            strip_mode[t1] = Waiting_hit;
            strip_mode[t2] = Waiting_hit;

            flag_first_hit   = 0;
            activation_timer = 0;
            BigRuneState     = STATE_WAITING;
        }
        break;

        case STATE_WAITING:
            // 2.5s 超时检测
            activation_timer++;
            if (activation_timer > TIME_OUT)
            {
                BigRuneState     = STATE_GENERATE;
                game_stage       = 0;
                activation_timer = 0;
                LeafReset();
                return;
            }

            // 检测击打
            if (flag_first_hit == 1)
            {
                game_stage++;
                bonus_window_timer = 0;
                flag_first_hit     = 0;
                activation_timer   = 0;
                BigRuneState       = STATE_BONUS_WINDOW;
            }
            break;

        case STATE_BONUS_WINDOW:
            bonus_window_timer++;
            int hit_cnt = 0;
            for (int i = 0; i < 5; i++)
            {
                if (strip_mode[i] == hit_finish) hit_cnt++;
            }
            if (hit_cnt == 2 || bonus_window_timer > BONUS_WINDOW_TIME)
            {
                BigRuneState = STATE_GENERATE;
            }
            break;
    }
}

//small rune :hit_finish almost=all_...，在hitDetect中作为flag
void Update_Target_Mode(void)
{
    if (rune_mode == BIG_RUNE)
    {
        for (int m = 0; m < 5; m++)
        {
            switch (strip_mode[m])
            {
                case Waiting_hit:
                    target_mode[m] = 1;
                    break;
                case hit_finish:
                    target_mode[m] = 0;
                    break;
                case LED_none:
                    target_mode[m] = 0;
                    break;
                case all_finish:
                    target_mode[m] = 2;
                    break;
            }
        }
    }
    if (rune_mode == SMALL_RUNE)
    {
        for (int m = 0; m < 5; m++)
        {
            switch (strip_mode[m])
            {
                case Waiting_hit:
                    target_mode[m] = 1;
                    break;
                case hit_finish:
                    target_mode[m] = 2;
                    break;
                case LED_none:
                    target_mode[m] = 0;
                    break;
                case all_finish:
                    target_mode[m] = 2;
                    break;
            }
        }
    }
}

//small rune :hit_finish almost=all_...，在hitDetect中作为flag
void Update_Strip_Buffers(void)
{
    if (rune_mode == BIG_RUNE)
    {
        for (int m = 0; m < 5; m++)
        {
            switch (strip_mode[m])
            {
                case Waiting_hit:
                    my_ws2812_set_all_off(m);
                    LED_currentAll(m);
                    target_mode[m] = 1;
                    break;
                case hit_finish:
                    my_ws2812_set_all_off(m); // 箭头流水全亮->熄灭
                    LED_Strip(m);
                    // 靶子全亮->none case
                    target_mode[m] = 0;
                    break;
                case LED_none:
                    my_ws2812_set_all_off(m);
                    LED_Strip(m);
                    target_mode[m] = 0;
                    break;
                case all_finish:
                    my_ws2812_set_all_off(m);
                    LED_current_all_on(m);
                    target_mode[m] = 2;
                    break;
            }
        }
    }
    if (rune_mode == SMALL_RUNE)
    {
        for (int m = 0; m < 5; m++)
        {
            switch (strip_mode[m])
            {
                case Waiting_hit:
                    my_ws2812_set_all_off(m);
                    LED_current(m);
                    target_mode[m] = 1;
                    break;
                case hit_finish:
                    my_ws2812_set_all_off(m);
                    LED_current_all_on(m);
                    target_mode[m] = 2;
                    break;
                case LED_none:
                    my_ws2812_set_all_off(m);
                    target_mode[m] = 0;
                    break;
                case all_finish:
                    my_ws2812_set_all_off(m);
                    LED_current_all_on(m);
                    target_mode[m] = 2;
                    break;
            }
        }
    }
}

// ================= MAIN LOOP =================

void buffTask_loop(void)
{
    //if (time_tick >0xFFFFFFF0)time_tick = 0;
    //当一个无符号整数超过它能表示的最大值时，它会直接回到 0，然后重新开始计数。
    time_tick++;

    if (flag_start == 0)
    {
        flag_start = 1;
        //  srand(time_tick); //generate seed for func rand
    }
    if (flag_punish == 1)
    {
        LeafReset();
        flag_punish = 0;
    }
    hitDetect();
    {
        if (rune_mode == SMALL_RUNE) SmallRuneLoop();
        if (rune_mode == BIG_RUNE) BigRuneLoop();
    }

    Update_Target_Mode();

    if (time_tick % 30 == 0)
    {
        animationFrame++;
        if (animationFrame > 8) animationFrame = 1;
    }

    if (time_tick % FlashInterval == 0)
    {
        Update_Strip_Buffers();
        for (int i = 0; i < 5; i++) transfer(i); // 发送WS2812数据
        //Target_Set_All(target_mode);
    }
}
