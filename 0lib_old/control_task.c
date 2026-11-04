#include "control_task.h"
#include <stdlib.h>
#include "delay.h"
#include "led.h" // ???????led?????????LED????
#include "water_flow.h"
#include "ws2812b.h" // ??????????????
// ????????????????????????????????????????
// transfer5,????????????tim3??ch2-PA7  ???????uint16_t ???��??
// transfer1,4????leaf1/4??tim5??ch1??ch4 =????pa0??pa3????uint32_t ????
// ??????PA7???????????��??2.25
// 1??can??2?????
// 2.26????????
// 3.8 1??pa0??2??pa3??1??pa3??
// a0_pb5??a3_pc12;2a3_pb3;a7_pc11;2a0_PB4
// 6.28 1??PA2???��????f1??????????

// ========== ?????????? (Master) ==========
#define dsplT 1000
int game_stage = 0; // ?????????? (0-5)
int bonus_window_timer = 0; // 1??????????????
uint8_t flag_first_hit = 0; // ????????????????
uint16_t reset_time_cnt = 0; // ????????
int g1 = 0;
int time_tick = 0;
int time1 = 0;
int led_time1 = 1;
int all_finish_time[5] = {0, 0, 0, 0, 0};
int last_led_time1 = 24;

int turn_flag = 0;
int energe_mode = 0;
int energe_leaf = 0;
long long time_Big_energe = 0;

int energe_leaf_all_rand_flag = 0;
leaf LED[5] = {0, 0, 0, 0, 0};
int current_leaf;
uint8_t n = 1;
uint8_t m;
uint8_t flag_start = 0;
uint8_t count_leaf = 0; // ?????
uint8_t flag_all_finish = 0;
uint8_t finish_flag = 0;
uint8_t rand_finish = 0;
uint8_t ready_to_reset = 0;

uint8_t target_mode[5] = {0, 0, 0, 0, 0}; // ???????
uint8_t led_mode_last[5] = {0};
uint8_t water_flow_flag = 0;
uint8_t water_flow_stepped = 0;
// 0?��????1???????2????
//->��????=???????=0,...
void led_display_set(uint8_t m)
{
    led_mode_last[m] = LED[m].mode;

    switch (LED[m].mode)
    {
    case Waiting_hit:
        target_mode[m] = 1;
        my_ws2812_set_all_off(m);
        water_flow_flag = 1;


        // LED_currentAll(m);  // ????????????????
        //  side_off(m);        // ?????
        break;
    case hit_finish:
        my_ws2812_set_all_off(m); // ?????????->???

        // side_off(m);
        //        // ???????->=none case

        LED_Strip(m);
        target_mode[m] = 0;
        break;
    case LED_none:
        my_ws2812_set_all_off(m);

        LED_Strip(m);
        target_mode[m] = 0;
        break;
    case all_finish:
        my_ws2812_set_all_off(m);

        LED_current_all_on(m); // ?????????,????
        //    side_off(m);            // ??????
        //      LED_circle(m);
        target_mode[m] = 2;
        break;
    }
    if (led_mode_last[m] == Waiting_hit && LED[m].mode != Waiting_hit)
    {
        WaterFlow_Stop();
    }
    if (water_flow_flag == 1 && water_flow_stepped == 0)
    {
        water_flow_stepped = 1;
        WaterFlow_Step(); // ?????????????
        WaterFlow_Display(); // ????LED???
        delay_us(10000);
        water_flow_flag = 0;
    }
    // transfer(m);
    if (time_tick % 50 == 0)
    {
        target_mode[0] = 1;
        target_mode[1] = 1;
        target_mode[2] = 1;
        target_mode[3] = 1;
        target_mode[4] = 1;
        uint8_t tx_buf[] = {0x40, target_mode[0], target_mode[1], target_mode[2], target_mode[3], target_mode[4]};
        USART2_SendArray(tx_buf, 6); // to ?????,???????
    } // ?????????????
}

// =========================================

void LED_task(void)
{
    if (time_tick % 50 == 0)
    {
        water_flow_stepped = 0;
        m = 0;
        led_display_set(m);
    }

    if (time_tick % 50 == 20)
    {
        m = 3;
        led_display_set(m);
        //     uint8_t tx_buf[] = {0x40,           target_mode[0], target_mode[1],
        //                     target_mode[2], target_mode[3], target_mode[4]};
        // USART2_SendArray(tx_buf, 6);
    }

    if (time_tick % 50 == 2)
    {
        m = 4;
        led_display_set(m);
        //     uint8_t tx_buf[] = {0x40,           target_mode[0], target_mode[1],
        //                     target_mode[2], target_mode[3], target_mode[4]};
        // USART2_SendArray(tx_buf, 6);
    }
    if (time_tick % 50 == 40)
    {
        m = 1;
        switch (LED[m].mode)
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
        // 	    uint8_t tx_buf[] = {0x40,           target_mode[0], target_mode[1],
        //                     target_mode[2], target_mode[3], target_mode[4]};
        // USART2_SendArray(tx_buf, 6);
    }
    if (time_tick % 50 == 42)
    {
        m = 2;
        switch (LED[m].mode)
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
        // 	    uint8_t tx_buf[] = {0x40,           target_mode[0], target_mode[1],
        //                     target_mode[2], target_mode[3], target_mode[4]};
        // USART2_SendArray(tx_buf, 6);
    }

    if (time_tick % 50 == 46)
    {
        uint8_t tx_buf[] = {0x40,           target_mode[0], target_mode[1], target_mode[2],
                            target_mode[3], target_mode[4], target_mode[5]};
        USART2_SendArray(tx_buf, 6); // to ?????,???????
    }
}

void control_task(void)
{
#if 0
    time_tick++;
    RC_CtrlData.rc.s2 = 0;
    flag_start = 1;
    // 1. CAN??????????��?????????????? game_stage
    CAN1_Send(LED[1].mode, LED[2].mode, n, game_stage, 0);
    //


    // ??????
    //
    if (flag_start == 1)
    {
        LED_task();
    }

    if (time_tick % 50 == 0)
    {
        time1++;
    }

    if (RC_CtrlData.rc.s2 == 0) // ?????
    {
        if (flag_start == 1)
        {
            srand(time_tick);
        }

        // ==================== [????] ????��???? ====================
        if (game_stage >= 5 && flag_first_hit == 0)
        {
            // ???????????????????
            for (int i = 0; i < 5; i++)
            {
                LED[i].mode = all_finish;
                target_mode[i] = 2;
            }
            // ????5???? (???? control_task ? 1ms ?????????)
            reset_time_cnt++;

            if (reset_time_cnt > dsplT)
            {
                game_stage = 0;
                reset_time_cnt = 0;
                flag_first_hit = 0;
                bonus_window_timer = 0;
                // ???????
                for (int i = 0; i < 5; i++)
                    LED[i].mode = LED_none;
            }

            // ????????????��?????????????????????
            LED_task();

            if (time_tick % 35 == 0)
            {
                led_time1++;
                if (led_time1 > 8)
                    led_time1 = 1;
            }
            // key_input();
            return;
        }

        // ==================== [???] ??????????? ====================
        // ???????????????????????
        int active_count = 0;
        for (int i = 0; i < 5; i++)
        {
            if (LED[i].mode == Waiting_hit)
                active_count++;
        }

        // ??????????????? ?? ?????????????? ?? ???????
        if (active_count == 0 && bonus_window_timer == 0 && flag_start == 1 && rand_finish == 0)
        {
            int t1 = rand() % 5;
            int t2 = rand() % 5;

            // ?????????????????????
            while (t1 == t2)
            {
                t2 = rand() % 5;
            }
            g1 = t1;
            LED[t1].mode = Waiting_hit;
            LED[t2].mode = Waiting_hit;

            flag_first_hit = 0; // ??????????
            count_reset = 0; // ???��??????
        }

        // ==================== [????] ?????????? ====================
        if (1)
        {
            if (flag_first_hit == 1)
            {
                bonus_window_timer++;
                // 1?? (1000??tick) ???????????????��??????????��???????
                if (bonus_window_timer >= 1000)
                {
                    for (int i = 0; i < 5; i++)
                    {
                        if (LED[i].mode == Waiting_hit || LED[i].mode == hit_finish) //"!"refresh"!" state
                            LED[i].mode = LED_none;
                    }
                    flag_first_hit = 0;
                    bonus_window_timer = 0;
                }
            }

            if (flag_start == 0)
            { // ??????????,????
                LED_task();
            }

            key_input(); // ???��???/???????

            if (time_tick % 35 == 0)
            {
                led_time1++;
                if (led_time1 > 8)
                    led_time1 = 1;
            }

            // if (time_tick % 50 == 0) {
            //   uint8_t tx_buf[] = {0x40,        LED[0].mode, LED[1].mode,
            //                       LED[2].mode, LED[3].mode, LED[4].mode};
            //   USART2_SendArray(tx_buf, 6);
            // }

            if (time_tick % 10000000 == 0)
                time_tick = 0;
        }
    }
#else
    for (int leaf = 0; leaf < 5; leaf++)
    {

    for (int j = 1; j <= 64*5; j++)
    {
        my_ws2812_1_set(leaf, j, 222, 0, 0);
    }

			    switch (leaf)
    {
    case 0:
        transfer1();
        break;
    case 3:
        transfer4();
        break;
    case 4:
        transfer5();
        break;
    }
    }
#endif
}

void clean_time(void)
{
    led_time1 = 1;
    last_led_time1 = 24;
}

void control_task_Init(void)
{
    // ?????????
}
