#include "led.h"
#include "sys.h"

// 引用外部控制变量
extern int game_stage;
extern int bonus_window_timer;
extern uint8_t flag_first_hit;

// 传感器状态变量
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

int flag_rc1;
int flag_rc11;
uint16_t count_reset = 0; // 2.5s 超时计时器

// LED 模式数组 (假设结构体定义在头文件中)
// LED_Class LED[5]; // 此处仅示意，实际定义需与头文件一致

void key_input(void)
{
    // ================= 1. 读取硬件 IO 状态 =================
    Flag_Input_1 = (Sucker_PB_5);
    Flag_Input_2 = (Sucker_PB_4);
    Flag_Input_3 = (Sucker_PB_3);
    Flag_Input_4 = (Sucker_PC_12);
    Flag_Input_5 = (Sucker_PC_11);
    Flag_Input_n = (Sucker_PA_1); // 调试/重置按钮

    // ================= 2. 调试模式 (虚拟击打) =================
    if (Flag_Input_n == 1)
    {
        flag_rc1 = 1;
    }
    if (Flag_Input_n == 0 && flag_rc1 == 1)
    {
        flag_rc11 = 1; // 产生一个虚拟触发信号
        flag_rc1 = 0;
    }

    // 如果触发调试信号，自动寻找一个 waiting 的目标并“击中”它
    if (flag_rc11 == 1)
    {
        for (int i = 0; i < 5; i++)
        {
            if (LED[i].mode == Waiting_hit)
            {
                // 强制模拟对应的传感器被触发
                if(i==0) { Flag_Input_1 = 1; Flag_Input_1_Last = 0; }
                if(i==1) { Flag_Input_2 = 1; Flag_Input_2_Last = 0; }
                if(i==2) { Flag_Input_3 = 1; Flag_Input_3_Last = 0; }
                if(i==3) { Flag_Input_4 = 1; Flag_Input_4_Last = 0; }
                if(i==4) { Flag_Input_5 = 1; Flag_Input_5_Last = 0; }
                flag_rc11 = 0; // 清除信号
                break; // 只打一个
            }
        }
    }

    // 将传感器整理为数组，方便循环判断
    int Sensors_Current[5] = {Flag_Input_1, Flag_Input_2, Flag_Input_3, Flag_Input_4, Flag_Input_5};
    int Sensors_Last[5]    = {Flag_Input_1_Last, Flag_Input_2_Last, Flag_Input_3_Last, Flag_Input_4_Last, Flag_Input_5_Last};

    // ================= 3. 击打判定逻辑 (裁判系统) =================
    for (int i = 0; i < 5; i++)
    {
        // 上升沿检测：从 0 变 1，表示刚刚被击中
        if (Sensors_Last[i] == 0 && Sensors_Current[i] == 1)
        {
            if (LED[i].mode == Waiting_hit)
            {
                // --- [A] 击中有效目标 ---
                LED[i].mode = hit_finish; // 改变灯效为“击中”
                count_reset = 0;          // 重置超时计时

                if (flag_first_hit == 0)
                {
                    // >>> 第一次击中（成功激活当前组） <<<
                    flag_first_hit = 1;
                    game_stage++;           // 进度 +1
                    bonus_window_timer = 0; // 开启 1秒 连击窗口
                }
                else
                {
                    // >>> 第二次击中（触发连击） <<<
                    // 立即结束本轮等待，让主循环立刻刷新下一组
                    bonus_window_timer = 1000; 
                }
            }
            else if (LED[i].mode == LED_none || LED[i].mode == all_finish)
            {
                // --- [B] 击中无效目标 (误击) ---
                // 规则：击中非随机点亮的装甲模块，激活失败
                game_stage = 0;
                // 惩罚性重置：设置计时器为最大值，触发下方的超时逻辑
                count_reset = 5000; 
            }
        }
    }

    // ================= 4. 超时判定逻辑 =================
    // 只有在“还没打中第一个”的时候，才计算 2.5秒 超时
    // 如果已经打中第一个，进入的是 1秒 Bonus 倒计时 (在 control_task 里处理)
    if (flag_first_hit == 0 && game_stage < 5)
    {
        // 检查场上是否有灯亮着
        int any_active = 0;
        for (int k = 0; k < 5; k++) 
        {
            if (LED[k].mode == Waiting_hit) any_active = 1;
        }

        if (any_active)
        {
            count_reset++;
            // 2.5秒 超时 (假设 key_input 频率为 1kHz/1ms)
            if (count_reset >= 2500)
            {
                // 超时，激活失败
                game_stage = 0;
                // 全场熄灯
                for (int k = 0; k < 5; k++) LED[k].mode = LED_none;
                count_reset = 0;
            }
        }
    }

    // ================= 5. 更新历史状态 =================
    Flag_Input_1_Last = Flag_Input_1;
    Flag_Input_2_Last = Flag_Input_2;
    Flag_Input_3_Last = Flag_Input_3;
    Flag_Input_4_Last = Flag_Input_4;
    Flag_Input_5_Last = Flag_Input_5;
}

// ---------------- 以下为初始化函数 (保持原样) ----------------

void LED_Init(void) { /* ... 原初始化代码 ... */ }
void Sucker_PC11_Init(void) { /* ... 原初始化代码 ... */ }
void Sucker_PC12_Init(void) { /* ... 原初始化代码 ... */ }
void Sucker_PB5_Init(void)  { /* ... 原初始化代码 ... */ }
void Sucker_PA1_Init(void)  { /* ... 原初始化代码 ... */ }
void Sucker_PB4_Init(void)  { /* ... 原初始化代码 ... */ }
void Sucker_PB3_Init(void)  { /* ... 原初始化代码 ... */ }