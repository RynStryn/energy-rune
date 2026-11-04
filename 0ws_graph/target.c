#include "target.h"
#include <string.h>


#define dma_buf_0 ((volatile uint16_t*) 0x30000000)
#define dma_buf_1 ((volatile uint16_t*) 0x30007800)
#define dma_buf_2 ((volatile uint16_t*) 0x3000F000)
#define dma_buf_3 ((volatile uint16_t*) 0x30016800)
#define dma_buf_4 ((volatile uint16_t*) 0x3001E000)


Target_t    g_target[5];
GameState_t g_game;


extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;



typedef struct {
    GPIO_TypeDef* port;
    uint16_t      pin;
} SensorPin_t;

static const SensorPin_t s_sensor[5] = {
    {GPIOB, GPIO_PIN_14},  // Leaf 0
    {GPIOC, GPIO_PIN_9},  // Leaf 1
    {GPIOD, GPIO_PIN_11},  // Leaf 2
    {GPIOC, GPIO_PIN_8},   // Leaf 3
    {GPIOD, GPIO_PIN_10},   // Leaf 4
};

// 坐标映射
extern const uint16_t map[PANEL_ROWS][PANEL_COLS];

/* 
 *  击打记录.log for debug
 *  */
volatile uint8_t g_hit_log[HIT_LOG_SIZE];
volatile uint8_t g_hit_log_cnt = 0;

void Target_RecordHit(uint8_t idx)
{
    if (idx >= 5) return;
    if (g_hit_log_cnt < HIT_LOG_SIZE)
        g_hit_log[g_hit_log_cnt++] = idx;
}

void Target_InitAll(void)
{
    g_target[0].strip.htim       = &htim1;
    g_target[0].strip.channel    = TIM_CHANNEL_1;
    g_target[0].strip.dma_buffer = dma_buf_0;
    g_target[0].strip.map        = map;
    g_target[0].face.port_b      = GPIOB;
    g_target[0].face.pin_b       = GPIO_PIN_7;
    g_target[0].face.port_a      = GPIOD;
    g_target[0].face.pin_a       = GPIO_PIN_0;

    g_target[1].strip.htim       = &htim3;
    g_target[1].strip.channel    = TIM_CHANNEL_2;
    g_target[1].strip.dma_buffer = dma_buf_1;
    g_target[1].strip.map        = map;
    g_target[1].face.port_a      = GPIOD;
    g_target[1].face.pin_a       = GPIO_PIN_4;
    g_target[1].face.port_b      = GPIOE;
    g_target[1].face.pin_b       = GPIO_PIN_1;

    g_target[2].strip.htim       = &htim3;
    g_target[2].strip.channel    = TIM_CHANNEL_3;
    g_target[2].strip.dma_buffer = dma_buf_2;
    g_target[2].strip.map        = map;
    g_target[2].face.port_b      = GPIOB;
    g_target[2].face.pin_b       = GPIO_PIN_9;
    g_target[2].face.port_a      = GPIOD;
    g_target[2].face.pin_a       = GPIO_PIN_2;

    g_target[3].strip.htim       = &htim4;
    g_target[3].strip.channel    = TIM_CHANNEL_1;
    g_target[3].strip.dma_buffer = dma_buf_3;
    g_target[3].strip.map        = map;
    g_target[3].face.port_b      = GPIOE;
    g_target[3].face.pin_b       = GPIO_PIN_0;
    g_target[3].face.port_a      = GPIOD;
    g_target[3].face.pin_a       = GPIO_PIN_3;

    g_target[4].strip.htim       = &htim1;
    g_target[4].strip.dma_buffer = dma_buf_4;
    g_target[4].strip.channel    = TIM_CHANNEL_4;
    g_target[4].strip.map        = map;
    g_target[4].face.port_b      = GPIOB;
    g_target[4].face.pin_b       = GPIO_PIN_8;
    g_target[4].face.port_a      = GPIOD;
    g_target[4].face.pin_a       = GPIO_PIN_1;
//2与5对调继电器引脚

    // 初始化所有 strip 和 face
    for (int i = 0; i < 5; i++)
    {
        Target_ClearStrip(i);
        Target_LoadLED(i);
        g_target[i].strip.state             = LED_none;
        g_target[i].strip.hit_timestamp    = 0;
        g_target[i].strip.last_input        = Target_ReadSensor(i);
        g_target[i].strip.animation_frame   = 1;
        g_target[i].strip.water_flow_active = 0;
        g_target[i].face.mode               = 0;
        Target_SetFace(i, 0);
    }

    // 初始化游戏状态
    memset(&g_game, 0, sizeof(g_game));
    g_game.rune_mode = 1;  // BIG_RUNE
}

/*
 *  mode: 0=熄灭, 1=中心(两路拉高), 2=全亮(A高B低)
 *  */
void Target_SetFace(uint8_t idx, uint8_t mode)
{
    if (idx >= 5)
        return;
    TargetFace_t* f = &g_target[idx].face;
    f->mode         = mode;
//全灭;全亮;中心;~=全灭
    switch (mode)
    {
        case 0:
            HAL_GPIO_WritePin(f->port_a, f->pin_a, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(f->port_b, f->pin_b, GPIO_PIN_RESET);
            break;
        case 1:
            HAL_GPIO_WritePin(f->port_a, f->pin_a, GPIO_PIN_SET);
            HAL_GPIO_WritePin(f->port_b, f->pin_b, GPIO_PIN_SET);
            break;
        case 2:
            HAL_GPIO_WritePin(f->port_a, f->pin_a, GPIO_PIN_SET);
            HAL_GPIO_WritePin(f->port_b, f->pin_b, GPIO_PIN_RESET);
            break;
        case 3:
            HAL_GPIO_WritePin(f->port_a, f->pin_a, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(f->port_b, f->pin_b, GPIO_PIN_SET);
            break;
        default:
            HAL_GPIO_WritePin(f->port_a, f->pin_a, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(f->port_b, f->pin_b, GPIO_PIN_RESET);
            break;
    }
}

/* 设置虚拟面板单颗灯珠 */
void Target_SetLed(uint8_t idx,
    uint8_t                x,
    uint8_t                y,
    uint8_t                r,
    uint8_t                g,
    uint8_t                b)
{
    if (idx >= 5 || x >= STRIP_COLS || y >= STRIP_ROWS)
        return;
    LEDStrip_t* s         = &g_target[idx].strip;
    s->panel[y][x].r      = r;
    s->panel[y][x].g      = g;
    s->panel[y][x].b      = b;
    s->panel[y][x].enable = 1;
    s->panel_change         = 1;
}

/* 清除虚拟面板
 *  */
void Target_ClearStrip(uint8_t idx)
{
    if (idx >= 5)
        return;
    memset(g_target[idx].strip.panel, 0, sizeof(g_target[idx].strip.panel));
    g_target[idx].strip.panel_change = 1;
}

volatile uint32_t dbg[16] = {0};


static uint32_t Ch2DmaId(uint32_t ch)
{
    if (ch == TIM_CHANNEL_1)
        return TIM_DMA_ID_CC1;
    if (ch == TIM_CHANNEL_2)
        return TIM_DMA_ID_CC2;
    if (ch == TIM_CHANNEL_3)
        return TIM_DMA_ID_CC3;
    return TIM_DMA_ID_CC4;
}

// static uint32_t Ch2DmaBit(uint32_t ch)
// {
//     if (ch == TIM_CHANNEL_1)
//         return TIM_DMA_CC1;
//     if (ch == TIM_CHANNEL_2)
//         return TIM_DMA_CC2;
//     if (ch == TIM_CHANNEL_3)
//         return TIM_DMA_CC3;
//     return TIM_DMA_CC4;
// }

static void Target_TransferSingle(LEDStrip_t* s)
{
    TIM_HandleTypeDef* htim    = s->htim;
    TIM_TypeDef*       tim     = htim->Instance;
    uint32_t           channel = s->channel;
    DMA_HandleTypeDef* hdma
        = (DMA_HandleTypeDef*) htim->hdma[Ch2DmaId(channel)];

    if (hdma == NULL)
    {
        return;
    }

    if (HAL_TIM_PWM_Start_DMA(htim, channel, (uint32_t*) s->dma_buffer,
                              DMA_BUFF_SIZE) != HAL_OK)
    {
        return;
    }

    while (hdma->State != HAL_DMA_STATE_READY)
    {
        __NOP();
    }

    tim->SR &= ~TIM_SR_UIF;
    while (!(tim->SR & TIM_SR_UIF))
    {
        __NOP();
    }
    tim->SR &= ~TIM_SR_UIF;

    HAL_TIM_PWM_Stop_DMA(htim, channel);
}
/* 
 逐路 DMA 发送
 *  */
LED_mode_type last_state[5];
uint8_t tick;
void Target_TransferAll(void)
{
    dbg[8]++;

   //  tick++;
   //  if (tick >= 10)
   //  {
   //      for (int i = 0; i < 5; i++)
   //      {last_state[i]=g_target[i].strip.state;}
   // tick=0;
   //  }


    for (int i = 0; i < 5; ++i) {
        //非流水灯 且与上次相同，则不用更新

        if (0) {
            Target_TransferNoDMA(i);
        } else {
            Target_TransferSingle(&g_target[i].strip);
        }
    }
    //Target_TransferSingle(&g_target[4].strip);
    dbg[9]++;
}

/* 只传输变化的strip
 *  */
void Target_TransferChangePart(void)
{
    for (int i = 0; i < 5; i++)
    {
        if (g_target[i].strip.panel_change)
        {
            Target_LoadLED(i);
            if (0)
            {
                __disable_irq();
                Target_TransferNoDMA(i);
                __enable_irq();
            }
            else
            {
            Target_TransferSingle(&g_target[i].strip);
            }
            g_target[i].strip.panel_change = 0;
        }
    }
}

/* 
将颜色值转换为 PWM
 *  */
static void Color2Pwm(uint8_t color, volatile uint16_t* pwm_buf)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        *pwm_buf++ = (color & (1 << i)) ? TIMING_ONE : TIMING_ZERO;
    }
}


// #if 0
// void Target_LoadLED(uint8_t idx) {
//   dbg[7]++;
//   if (idx >= 5)
//     return;
//   LEDStrip_t* s = &g_target[idx].strip;
//
//   memset(s->grb_buffer, 0, sizeof(s->grb_buffer));
//   for (uint8_t y = 0; y < STRIP_ROWS; y++) {
//     for (uint8_t x = 0; x < STRIP_COLS; x++) {
//       if (s->panel[y][x].enable) {
//         uint16_t led_idx = s->map[y][x];
//         s->grb_buffer[led_idx * 3 + 0] = s->panel[y][x].g;
//         s->grb_buffer[led_idx * 3 + 1] = s->panel[y][x].r;
//         s->grb_buffer[led_idx * 3 + 2] = s->panel[y][x].b;
//       }
//     }
//   }
//
//   volatile uint16_t* p_buf = s->dma_buffer;
//   for (int i = 0; i < DMA_BUFF_SIZE; i++) {
//     *p_buf++ = 0;
//   }
//   p_buf = s->dma_buffer;
//   for (int i = 0; i < STRIP_LEDS_PER_LEAF; i++) {
//     Color2Pwm(s->grb_buffer[i * 3 + 0], p_buf);
//     p_buf += 8;
//     Color2Pwm(s->grb_buffer[i * 3 + 1], p_buf);
//     p_buf += 8;
//     Color2Pwm(s->grb_buffer[i * 3 + 2], p_buf);
//     p_buf += 8;
//   }
// }
// #endif


/*
虚拟面板 -> GRB buffer -> DMA buffer
 *  */
void Target_LoadLED(uint8_t idx)
{
    dbg[7]++;
    if (idx >= 5)
        return;
    LEDStrip_t* s = &g_target[idx].strip;

    memset(s->grb_buffer, 0, sizeof(s->grb_buffer));
    for (uint8_t y = 0; y < STRIP_ROWS; y++)
    {
        for (uint8_t x = 0; x < STRIP_COLS; x++)
        {
            if (s->panel[y][x].enable)
            {
                uint16_t led_idx               = s->map[y][x];
                s->grb_buffer[led_idx * 3 + 0] = s->panel[y][x].g;
                s->grb_buffer[led_idx * 3 + 1] = s->panel[y][x].r;
                s->grb_buffer[led_idx * 3 + 2] = s->panel[y][x].b;
            }
        }
    }

    // 2. GRB buffer -> DMA buffer (PWM 编码)
    memset(s->dma_buffer, 0, DMA_BUFF_SIZE * sizeof(uint16_t));
    uint16_t* p_buf = s->dma_buffer;
    for (int i = 0; i < STRIP_LEDS_PER_LEAF; i++)
    {
        Color2Pwm(s->grb_buffer[i * 3 + 0], p_buf);
        p_buf += 8;
        Color2Pwm(s->grb_buffer[i * 3 + 1], p_buf);
        p_buf += 8;
        Color2Pwm(s->grb_buffer[i * 3 + 2], p_buf);
        p_buf += 8;
    }
    s->panel_change = 0;
}

/* 接受指针的版本 */
void Target_LoadLEDPtr(LEDStrip_t* s)
{
    memset(s->grb_buffer, 0, sizeof(s->grb_buffer));
    for (uint8_t y = 0; y < STRIP_ROWS; y++)
    {
        for (uint8_t x = 0; x < STRIP_COLS; x++)
        {
            if (s->panel[y][x].enable)
            {
                uint16_t led_idx               = s->map[y][x];
                s->grb_buffer[led_idx * 3 + 0] = s->panel[y][x].g;
                s->grb_buffer[led_idx * 3 + 1] = s->panel[y][x].r;
                s->grb_buffer[led_idx * 3 + 2] = s->panel[y][x].b;
            }
        }
    }

    volatile uint16_t* p_buf = s->dma_buffer;
    for (int i = 0; i < DMA_BUFF_SIZE; i++)
    {
        *p_buf++ = 0;
    }
    p_buf = s->dma_buffer;
    for (int i = 0; i < STRIP_LEDS_PER_LEAF; i++)
    {
        Color2Pwm(s->grb_buffer[i * 3 + 0], p_buf);
        p_buf += 8;
        Color2Pwm(s->grb_buffer[i * 3 + 1], p_buf);
        p_buf += 8;
        Color2Pwm(s->grb_buffer[i * 3 + 2], p_buf);
        p_buf += 8;
    }
    s->panel_change = 0;
}

/* 
读取传感器状态 (1=触发, 0=未触发)
 *  */
uint8_t Target_ReadSensor(uint8_t idx)
{
    if (idx >= 5)
        return 0;
    GPIO_PinState state
        = HAL_GPIO_ReadPin(s_sensor[idx].port, s_sensor[idx].pin);
    return (state == GPIO_PIN_SET) ? 1 : 0;
}

/* 灯板点亮测试
 *  每块 Leaf 显示不同纯色，依次点亮
 */


void strip_SequentialTest(void)
{
    static const uint8_t colors[5][3] = {
        {255, 0, 0},    // 0: 红
        {0, 255, 0},    // 1: 绿
        {0, 0, 255},    // 2: 蓝
        {255, 255, 0},  // 3: 黄
        {255, 0, 255},  // 4: 紫
    };

    for (int i = 0; i < 5; i++)
    {
        Target_ClearStrip(i);
    }

    for (int i = 0; i < 5; i++)
    {
        // 填满当前 Leaf 为纯色
        for (uint8_t y = 0; y < STRIP_ROWS; y++)
        {
            for (uint8_t x = 0; x < STRIP_COLS; x++)
            {
                Target_SetLed(
                    i, x, y, colors[i][0], colors[i][1], colors[i][2]);
            }
        }
        Target_LoadLED(i);
    }
static uint16_t time=0;
        if (time == 0)
        {
            Target_TransferAll();
            // Target_TransferNoDMA(1);
            // Target_TransferNoDMA(4);
            // Target_TransferNoDMA(0);
            // Target_TransferNoDMA(2);
            // Target_TransferNoDMA(3 );


            // Target_TransferSingle(&g_target[1].strip);
            // Target_TransferSingle(&g_target[4].strip);
        // if (time >= 60000)
        // {
        //     Target_ClearStrip(i);
        //     Target_LoadLED(i);
        //     Target_TransferNoDMA(1);
        //     Target_TransferNoDMA(4);
        //     time=0;
        // }
    }
}

/* CPU直驱
 *  */
void Target_TransferNoDMA(uint8_t idx)
{
    if (idx >= 5)
        return;

    LEDStrip_t*       s     = &g_target[idx].strip;
    TIM_HandleTypeDef* htim = s->htim;
    TIM_TypeDef*       tim  = htim->Instance;
    uint32_t           ch   = s->channel;

    __IO uint32_t* ccr;
    switch (ch)
    {
        case TIM_CHANNEL_1: ccr = &tim->CCR1; break;
        case TIM_CHANNEL_2: ccr = &tim->CCR2; break;
        case TIM_CHANNEL_3: ccr = &tim->CCR3; break;
        case TIM_CHANNEL_4: ccr = &tim->CCR4; break;
        default: return;
    }

    Target_LoadLED(idx);

    *ccr = s->dma_buffer[0];
    TIM_CCxChannelCmd(tim, ch, TIM_CCx_ENABLE);
    if (IS_TIM_ADVANCED_INSTANCE(tim))
        tim->BDTR |= TIM_BDTR_MOE;

    tim->CNT = 0;
    __HAL_TIM_ENABLE(htim);

    for (int i = 1; i < DMA_BUFF_SIZE; i++)
    {
        tim->SR &= ~TIM_SR_UIF;
        while (!(tim->SR & TIM_SR_UIF))
        {
            __NOP();
        }
        *ccr = s->dma_buffer[i];
    }

    tim->SR &= ~TIM_SR_UIF;
    while (!(tim->SR & TIM_SR_UIF))
    {
        __NOP();
    }
    tim->SR &= ~TIM_SR_UIF;

    __HAL_TIM_DISABLE(htim);
    TIM_CCxChannelCmd(tim, ch, TIM_CCx_DISABLE);
    if (IS_TIM_ADVANCED_INSTANCE(tim))
        tim->BDTR &= ~TIM_BDTR_MOE;
}

/*  测试靶面继电器
 *  依次: 熄灭->中心->全亮->圆周
 *  */
void Target_FaceTest(void)
{
    const uint8_t modes[] = {0, 1, 2, 3};

    while (1)
    {
        for (int m = 0; m < 4; m++)
        {
            for (int i = 0; i < 5; i++)
            {
                Target_SetFace(i, modes[m]);
            }
            for (volatile uint32_t d = 0; d < 80000000; d++)
            {
                __NOP();
            }
        }
    }
}
