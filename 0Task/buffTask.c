#include "buffTask.h"
#include <stdlib.h>
#include <string.h>
#include "software_config.h"
#include "stm32h7xx_hal.h"
#include "target.h"
#include "water_flow.h"

extern RNG_HandleTypeDef hrng;
extern volatile uint32_t dbg[16];

//#define USE_4_TARGETS
#if defined(USE_4_TARGETS)
static const uint8_t buff_indices[] = {0, 1,2, 3};
#define RAND_TARGET(r)  (buff_indices[(r) % 4])
#else
#define RAND_TARGET(r)  ((r) % 5)
#endif

void hitDetect(uint32_t now)
{
    for (int i = 0; i < 5; i++)
    {
        LEDStrip_t* s = &g_target[i].strip;

        uint8_t current_input = Target_ReadSensor(i);

        if (s->last_input == 0 && current_input == 1)
        {
            Target_RecordHit(i);
            if (s->state == Waiting_hit)
            {
                s->state              = hit_finish;
                s->hit_timestamp      = now + LED_PROTECTION_MS;
                g_game.flag_first_hit = 1;
            }
            else if ((int32_t)(now - s->hit_timestamp) >= 0)
            {
                if (s->state == LED_none || s->state == hit_finish)
                {
                    g_game.flag_punish = 1;
                }
            }
        }

        s->last_input = current_input;
    }
}
static void rune_reset(void)
{
    g_game.success_start = 0;
    for (int i = 0; i < 5; i++)
    {
        g_target[i].strip.state = LED_none;
        g_target[i].face.mode   = 0;
    }
    g_game.game_stage     = 0;
    g_game.flag_first_hit = 0;
    g_game.bonus_start    = 0;
    g_game.big_rune_state = STATE_GENERATE;
}
//  SMALL_RUNE
static void SmallRuneLoop(uint32_t now)
{
    if (g_game.game_stage >= 5)
    {
        if (g_game.success_start == 0)
            g_game.success_start = now;
        if ((uint32_t) (now - g_game.success_start) > SUCCESS_SHOW_MS)
        {
            rune_reset();
        }
        else
        {
            for (int m = 0; m < 5; m++)
            {
                if (g_target[m].strip.state != all_finish)
                    g_target[m].strip.state = all_finish;
                g_target[m].face.mode = 2;
            }
        }
        return;
    }

    int active_index = -1;
    for (int i = 0; i < 5; i++)
    {
        if (g_target[i].strip.state == Waiting_hit
            || g_target[i].strip.state == hit_finish)
        {
            active_index = i;
            break;
        }
    }

    if (active_index == -1)
    {
        uint32_t randSmall;
        if (HAL_RNG_GenerateRandomNumber(&hrng, &randSmall) != HAL_OK)
        {
            randSmall = 0x7FFFFFFF;
        }
        int t1   = RAND_TARGET(randSmall);
        int safe = 0;
        while (g_target[t1].strip.state == all_finish
            && safe < generationTimeLimit)
        {
            HAL_RNG_GenerateRandomNumber(&hrng, &randSmall);
            t1 = RAND_TARGET(randSmall);
            safe++;
        }
        // t1=0;
        if (g_target[t1].strip.state != all_finish)
        {
            g_target[t1].strip.state = Waiting_hit;
            g_game.activation_start  = now;
        }
    }
    else
    {
        if (g_target[active_index].strip.state == hit_finish)
        {
            g_game.game_stage++;
            g_target[active_index].strip.state = all_finish;
            g_game.activation_start            = now;
        }
        else
        {
            if ((uint32_t)(now - g_game.activation_start) > TIME_OUT_MS)
            {
                for (int i = 0; i < 5; i++)
                {
                    g_target[i].strip.state = LED_none;
                    g_target[i].face.mode   = 0;
                }
                g_game.game_stage        = 0;
                g_game.flag_first_hit    = 0;
                g_game.bonus_start       = 0;
                g_game.activation_start  = now;
            }
        }
    }
}


//  BIG_RUNE
static void BigRuneLoop(uint32_t now)
{
    if (g_game.game_stage >= 6)
    {
        if (g_game.success_start == 0)
            g_game.success_start = now;
        if ((uint32_t)(now - g_game.success_start) > SUCCESS_SHOW_MS)
        {
            rune_reset();
        }
        else
        {
            for (int m = 0; m < 5; m++)
            {
                g_target[m].strip.state = all_finish;
                g_target[m].face.mode   = 2;
            }
        }
        return;
    }

    switch (g_game.big_rune_state)
    {
        case STATE_GENERATE:
        {
            // 清理所有 Leaf，防止残留状态干扰新一轮
            for (int i = 0; i < 5; i++)
            {
                g_target[i].strip.state = LED_none;
                g_target[i].face.mode   = 0;
                if (g_target[i].strip.water_flow_active)
                    WaterFlow_Stop(&g_target[i].strip);
            }

            uint32_t rand1, rand2;
            HAL_RNG_GenerateRandomNumber(&hrng, &rand1);
            HAL_RNG_GenerateRandomNumber(&hrng, &rand2);
            int t1   = RAND_TARGET(rand1);
            int t2   = RAND_TARGET(rand2);
            int safe = 0;
            while (t1 == t2 && safe < generationTimeLimit)
            {
                HAL_RNG_GenerateRandomNumber(&hrng, &rand2);
                t2 = RAND_TARGET(rand2);
                safe++;
            }
            if (safe>=generationTimeLimit)
            {
                //
            }

            g_target[t1].strip.state = Waiting_hit;
            g_target[t2].strip.state = Waiting_hit;

            g_game.flag_first_hit   = 0;
            g_game.activation_start = now;
            g_game.big_rune_state   = STATE_WAITING;


        }

        break;

        case STATE_WAITING:
            if ((uint32_t)(now - g_game.activation_start) > TIME_OUT_MS)
            {
                g_game.big_rune_state   = STATE_GENERATE;
                g_game.game_stage       = 0;
                g_game.activation_start = now;
                for (int i = 0; i < 5; i++)
                {
                    g_target[i].strip.state = LED_none;
                    g_target[i].face.mode   = 0;
                }
                g_game.flag_first_hit = 0;
                g_game.bonus_start    = 0;
                return;
            }
            if (g_game.flag_first_hit == 1)
            {
                g_game.game_stage++;
                g_game.bonus_start      = 0;
                g_game.flag_first_hit   = 0;
                g_game.activation_start = now;
                g_game.big_rune_state   = STATE_BONUS_WINDOW;
            }
            break;

        case STATE_BONUS_WINDOW:
            if (g_game.bonus_start == 0)
                g_game.bonus_start = now;
            int hit_cnt = 0;
            for (int i = 0; i < 5; i++)
            {
                if (g_target[i].strip.state == hit_finish)
                    hit_cnt++;
            }
            if (hit_cnt == 2
                || (uint32_t)(now - g_game.bonus_start) > BONUS_WINDOW_MS)
            {
                // 最后一关已经完成一次命中
                // 等待BONUS窗口结束后再进入结算
                if (g_game.game_stage == 5)
                {
                    g_game.game_stage = 6;
                }

                g_game.big_rune_state = STATE_GENERATE;
            }
            break;
    }
}

// target_mode -> g_target[].face.mode
static void Game_UpdateTargetFaces(void)
{
    for (int m = 0; m < 5; m++)
    {
        switch (g_target[m].strip.state)
        {
            case Waiting_hit:
                g_target[m].face.mode = 1;
                break;
            case hit_finish:
            case LED_none:
                g_target[m].face.mode = 0;
                break;
            case all_finish:
                g_target[m].face.mode = 2;
                break;
        }
    }
}

//  灯效渲染
void Game_UpdateStripEffects(void)
{
    static LED_mode_type prev[5]
        = {LED_none, LED_none, LED_none, LED_none, LED_none};
    static uint8_t prev_stage[5]
        = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    for (int leaf = 0; leaf < 5; leaf++)
    {
        LEDStrip_t*   s         = &g_target[leaf].strip;
        LED_mode_type old_state = prev[leaf];

        if (old_state != s->state)
        {
            if (old_state == Waiting_hit)
            {
                WaterFlow_Stop(s);
                prev_stage[leaf] = 0xFF;
            }
            prev[leaf] = s->state;
        }

        switch (s->state)
        {
            case Waiting_hit:
                if (!WaterFlow_IsActive(s))
                    WaterFlow_StartArrow(s, COLOR_RED);
                s->panel_change = 1;
                break;
            case LED_none:
            case hit_finish:
            {
                if (prev_stage[leaf] == g_game.game_stage)
                    break;
                prev_stage[leaf] = g_game.game_stage;
                Target_ClearStrip(leaf);
                for (int j = 0; j < g_game.game_stage; j++)
                {
                    for (int y = 0; y < 8; y++)
                    {
                        for (int x = j * 8; x < (j + 1) * 8; x++)
                        {
                            if (x < STRIP_COLS)
                            {
                                s->panel[y][x].r      = 222;
                                s->panel[y][x].g      = 0;
                                s->panel[y][x].b      = 0;
                                s->panel[y][x].enable = 1;
                            }
                        }
                    }
                }
                s->panel_change = 1;
                break;
            }
            case all_finish:
            {
                if (old_state == all_finish)
                    break;
                memset(s->panel, 0, sizeof(s->panel));
                for (int y = 0; y < STRIP_ROWS; y++)
                {
                    for (int x = 0; x < STRIP_COLS; x++)
                    {
                        s->panel[y][x].r      = 222;
                        s->panel[y][x].g      = 0;
                        s->panel[y][x].b      = 0;
                        s->panel[y][x].enable = 1;
                    }
                }
                s->panel_change = 1;
                break;
            }
        }
    }
}


void buffTask_Init(void)
{

}

// 测试流水灯 (TIM1_CH1,逐列扫描)
static void Test_LED_Chaser(void)
{
    static uint8_t       pos         = 0;
    static uint8_t       index        = 0;
    static uint32_t      last_tick   = 0;
    static const uint8_t colors[][3] = {{255, 0, 0},
        {0, 255, 0},
        {0, 0, 255},
        {255, 255, 0},
        {0, 255, 255},
        {255, 0, 255}};

    uint32_t now = DWT->CYCCNT;
    if ((uint32_t)(now - last_tick) < LED_REFRESH_PERIOD)
        return;
    last_tick = now;
    {
        Target_ClearStrip(0);
        for (int y = 0; y < STRIP_ROWS; y++)
        {
            Target_SetLed(
                0, pos, y, colors[index][0], colors[index][1], colors[index][2]);
        }
        Target_LoadLED(0);

        pos++;
        if (pos >= STRIP_COLS)
        {
            pos = 0;
            if (++index >= sizeof(colors) / sizeof(colors[0]))
                index = 0;
        }
    }
}

void buffTask_loop(uint32_t now)
{
    if (g_game.flag_punish == 1)
    {
        for (int i = 0; i < 5; i++)
        {
            g_target[i].strip.state = LED_none;
            g_target[i].face.mode   = 0;
            WaterFlow_Stop(&g_target[i].strip);
        }
        g_game.game_stage       = 0;
        g_game.flag_first_hit   = 0;
        g_game.flag_punish      = 0;
        g_game.bonus_start      = 0;
        g_game.activation_start = now;
        g_game.success_start    = 0;
        g_game.big_rune_state   = STATE_GENERATE;
    }

    hitDetect(now);
if (reset_flag==1)
{
    reset_flag = 0;
    rune_reset();
}
    if (g_game.rune_mode == 0)
        SmallRuneLoop(now);
    if (g_game.rune_mode == 1)
        BigRuneLoop(now);

    Game_UpdateTargetFaces();

    if ((uint32_t)(now - g_game.animation_last) >= LED_ANIM_PERIOD)
    {
        g_game.animation_last = now;
        for (int i = 0; i < 5; i++)
        {
            g_target[i].strip.animation_frame++;
            if (g_target[i].strip.animation_frame > 8)
                g_target[i].strip.animation_frame = 1;
        }
    }
}
