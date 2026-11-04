#include "led.h"
#include "hardware_config.h" // 包含硬件引脚定义
#include "WS2812B.h"

// 引用全局变量
extern uint8_t strip_mode[5];
extern uint8_t flag_first_hit;
extern int     game_stage;
extern int     flag_punish;
// 历史状态记录
uint8_t Last_Input_State[5] = {0};
// 击打保护计时器 (防抖)
static int hit_protection_timer[5] = {0};
#define PROTECTION_PERIOD 50 //  保护期

// 读取指定叶片的传感器状态
// 返回 1 表示被击中(按下), 0 表示未击中
//TODO
//可能反了,注意一下
static uint8_t Get_Sensor_State(int leaf_index)
{
    GPIO_PinState bit_status = GPIO_PIN_RESET;
    switch (leaf_index)
    {
        // 这里的 GPIO_ReadInputDataBit 改为 HAL_GPIO_ReadPin
        case 0: bit_status = HAL_GPIO_ReadPin(SENSOR_1_PORT, SENSOR_1_PIN); break;
        case 1: bit_status = HAL_GPIO_ReadPin(SENSOR_2_PORT, SENSOR_2_PIN); break;
        case 2: bit_status = HAL_GPIO_ReadPin(SENSOR_3_PORT, SENSOR_3_PIN); break;
        case 3: bit_status = HAL_GPIO_ReadPin(SENSOR_4_PORT, SENSOR_4_PIN); break;
        case 4: bit_status = HAL_GPIO_ReadPin(SENSOR_5_PORT, SENSOR_5_PIN); break;
        default: return 0;
    }
    return (bit_status == GPIO_PIN_SET) ? 1 : 0;
}

void hitDetect(void)
{
    for (int i = 0; i < 5; i++)
    {
        if (hit_protection_timer[i] > 0)
        {
            hit_protection_timer[i]--;
        }


        uint8_t current_input = Get_Sensor_State(i);

        // 3. 上升沿检测 (从0变1)
        if (Last_Input_State[i] == 0 && current_input == 1)
        {
            // 情况 A: 打对了有奖励
            if (strip_mode[i] == Waiting_hit)
            {
                strip_mode[i]           = hit_finish;
                hit_protection_timer[i] = PROTECTION_PERIOD; // 开启保护期
                //通知 buffTask
                flag_first_hit = 1;
            }
            // 情况 B: 打错了有惩罚

            else if (hit_protection_timer[i] == 0)
            {
                if (strip_mode[i] == LED_none || strip_mode[i] == hit_finish)
                {
                    flag_punish = 1;
                }
            }
        }

        // 4. 更新历史
        Last_Input_State[i] = current_input;
    }
}


void LED_Init(void)
{
    // 保留空函数防报错
}
