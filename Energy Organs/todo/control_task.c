#include "sys.h"
#include "led.h"
#include "ws2812b.h"
#include <stdlib.h> // for rand, srand

/* ================= 全局变量定义 ================= */
// 游戏逻辑控制变量
int game_stage = 0;           // 当前激活进度 (0-5)
int bonus_window_timer = 0;   // 1秒连击窗口计时器
uint8_t flag_first_hit = 0;   // 标记：本轮是否已经击中过第一个目标
uint16_t time_tick = 0;       // 系统时基
uint16_t reset_time_cnt = 0;  // 通关后的展示倒计时
uint8_t flag_start = 0;       // 激活开始标志
int n = 0;                    // 临时变量

// 引入外部变量
extern RC_Ctrl_t RC_CtrlData; // 遥控器数据

void control_task(void)
{
    time_tick++;

    // 1. 发送 CAN 信号 (保持原逻辑，用于向裁判系统或云台发送状态)
    if (time_tick % 50 == 0)
    {
        // 假设 CAN1_Send 是现有的底层函数
        // CAN1_Send(LED[1].mode, LED[2].mode, n, 0, 0); 
    }

    // 2. 初始化随机种子 (收到开始信号时)
    if (flag_start == 1)
    {
        // 仅执行一次初始化
        // srand(time_tick); 
        // 实际应用中建议放到初始化函数里，或者确保只跑一次
        LED_task(); // 刷新灯效
    }

    // 3. 辅助计时
    if (time_tick % 50 == 0)
    {
        // time1++; 
    }

    // ============================================================
    // 核心业务逻辑 (当遥控器开关 S2 拨下时运行)
    // ============================================================
    if (RC_CtrlData.rc.s2 == 0) // 总开关
    {
        // [A] 通关判定 (Win Condition)
        // 如果进度达到 5 (击破 5 组)，视为通关
        if (game_stage >= 5)
        {
            // 强制点亮所有灯为“全亮状态”
            for (int i = 0; i < 5; i++)
            {
                LED[i].mode = all_finish;
            }

            // 通关展示逻辑：保持全亮 5 秒后重置
            reset_time_cnt++;
            if (reset_time_cnt > 5000) // 5000ms = 5s
            {
                game_stage = 0;
                reset_time_cnt = 0;
                flag_first_hit = 0;
                bonus_window_timer = 0;
                // 全灭，重新开始
                for (int i = 0; i < 5; i++) LED[i].mode = LED_none;
            }

            // 通关期间直接刷新灯效并返回，不生成新目标
            LED_task();
            key_input(); // 依然调用input以防需要手动重置
            return;
        }

        // [B] 生成双目标 (New Target Generation)
        // 条件：当前场上没有待打击目标 (active_count == 0) 且 不在连击窗口期
        int active_count = 0;
        for (int i = 0; i < 5; i++) {
            if (LED[i].mode == Waiting_hit) active_count++;
        }

        if (active_count == 0 && bonus_window_timer == 0 && flag_start == 1)
        {
            // 随机生成两个不同的目标索引 (0-4)
            int t1 = rand() % 5;
            int t2 = rand() % 5;

            // 防重逻辑：确保 t1 != t2
            // 且 (可选) 确保不选到正在播放通关动画的扇叶（如果需要）
            while (t1 == t2)
            {
                t2 = rand() % 5;
            }

            // 点亮这两个目标
            LED[t1].mode = Waiting_hit;
            LED[t2].mode = Waiting_hit;

            // 重置本轮状态
            flag_first_hit = 0;
            count_reset = 0; // 重置 2.5s 超时计时器
        }

        // [C] 处理 1秒 连击窗口 (Bonus Window)
        // 当击中第一个目标后，flag_first_hit 置 1，进入此逻辑
        if (flag_first_hit == 1)
        {
            bonus_window_timer++;
            // 如果 1秒 (1000ms) 结束
            if (bonus_window_timer >= 1000)
            {
                // 强制结束本轮：把剩下的那个 Waiting_hit 关掉
                for (int i = 0; i < 5; i++) {
                    if (LED[i].mode == Waiting_hit) {
                        LED[i].mode = LED_none; // 没打中就灭掉，不算失败，只是没拿到连击
                    }
                }
                // 清除状态，准备生成下一组
                flag_first_hit = 0;
                bonus_window_timer = 0;
            }
        }

        // [D] 调用裁判系统与灯效刷新
        key_input(); // 检测击打
        LED_task();  // 刷新视觉

        // [E] 数据回传
        if (time_tick % 50 == 0)
        {
            uint8_t tx_buf[] = {0x40, LED[0].mode, LED[1].mode, LED[2].mode, LED[3].mode, LED[4].mode};
            USART2_SendArray(tx_buf, 6);
        }

        // 简单的tick溢出保护
        if (time_tick % 10000000 == 0) time_tick = 0;
    }
}

// 辅助函数
void clean_time(void)
{
    led_time1 = 1;
    // last_led_time1 = 24; 
}

void control_task_Init(void)
{
    // 初始化代码
}

// LED 任务调度器 (负责调用底层的画图函数)
void LED_task(void)
{
    // 为了防止 DMA 拥堵，分时段刷新不同的扇叶
    // m=0 (0ms), m=3 (20ms), m=4 (2ms) ... 
    
    // 刷新扇叶 0
    if (time_tick % 50 == 0)
    {
        int m = 0;
        switch (LED[m].mode)
        {
        case Waiting_hit:
            LED_current(m);      // 流水灯
            side_off(m);         // 侧边关
            LED_target(m);       // 靶心图案
            break;
        case hit_finish:
            LED_current_all_on(m); // 全亮
            side_off(m);
            LED_circle(m);       // 【重点】这里会调用带进度条的函数
            break;
        case LED_none:
            my_ws2812_set_all_off(m);
            break;
        case all_finish:
            LED_current_all_on(m);
            side_off(m);
            LED_circle(m);       // 全亮进度条
            break;
        }
        transfer(m); // 发送数据
    }

    // 刷新扇叶 3
    if (time_tick % 50 == 20)
    {
        int m = 3;
        switch (LED[m].mode)
        {
        case Waiting_hit:
            LED_current(m);
            side_off(m);
            LED_target(m);
            break;
        case hit_finish:
            LED_current_all_on(m);
            side_off(m);
            LED_circle(m);
            break;
        case LED_none:
            my_ws2812_set_all_off(m);
            break;
        case all_finish:
            LED_current_all_on(m);
            side_off(m);
            LED_circle(m);
            break;
        }
        transfer(m);
    }

    // 刷新扇叶 4
    if (time_tick % 50 == 2)
    {
        int m = 4;
        switch (LED[m].mode)
        {
        case Waiting_hit:
            LED_current(m);
            side_off(m);
            LED_target(m);
            break;
        case hit_finish:
            LED_current_all_on(m);
            side_off(m);
            LED_circle(m);
            break;
        case LED_none:
            my_ws2812_set_all_off(m);
            break;
        case all_finish:
            LED_current_all_on(m);
            side_off(m);
            LED_circle(m);
            break;
        }
        transfer(m);
    }
    
    // 注意：原代码似乎漏了 1 和 2 号扇叶的 case，如果需要请自行补充，逻辑同上
}