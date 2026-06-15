#ifndef TARGET_H
#define TARGET_H

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "led_panel.h"
#include "map.h"

/* ============================================================
 *  常量定义
 * ============================================================ */
#define STRIP_LEDS_PER_LEAF  320
#define STRIP_ROWS            8
#define STRIP_COLS           40
#define DMA_BUFF_SIZE        (STRIP_LEDS_PER_LEAF * 24 * 2)  // 15360

// WS2812B 时序 (TIM period = 299, 240MHz TIM clock)
#define TIMING_ONE   180
#define TIMING_ZERO   72

/* ============================================================
 *  枚举
 * ============================================================ */
typedef enum {
    LED_none = 0,
    Waiting_hit,
    hit_finish,
    all_finish
} LED_mode_type;

typedef enum {
    STATE_GENERATE,
    STATE_WAITING,
    STATE_BONUS_WINDOW
} eBigRuneState;

/* ============================================================
 *  靶面 (继电器控制)
 * ============================================================ */
typedef struct {
    GPIO_TypeDef *port_a;
    uint16_t      pin_a;
    GPIO_TypeDef *port_b;
    uint16_t      pin_b;
    uint8_t       mode;     // 0=熄灭, 1=中心, 2=全亮, 3=圆周
} TargetFace_t;

/* ============================================================
 *  灯带 (WS2812B 灯效)
 * ============================================================ */
typedef struct {
    // --- 硬件绑定 ---
    TIM_HandleTypeDef *htim;
    uint32_t           channel;
    volatile uint16_t *dma_buffer;   // [DMA_BUFF_SIZE]

    // --- 虚拟面板 (8×40) ---
    Led_t     panel[STRIP_ROWS][STRIP_COLS];
    uint8_t   grb_buffer[STRIP_LEDS_PER_LEAF * 3];  // 960 字节

    // --- 坐标映射 (8×40 → 物理索引) ---
    const uint16_t (*map)[STRIP_COLS];

    // --- 状态 ---
    LED_mode_type state;
    uint32_t      hit_timestamp;      // 命中保护到期时间
    uint8_t       last_input;
    uint8_t       animation_frame;
    uint8_t       water_flow_active;  // 水流动画激活标志
    uint8_t       panel_change;        // 面板变化标记
} LEDStrip_t;

/* ============================================================
 *  靶子 (统一结构)
 * ============================================================ */
typedef struct {
    TargetFace_t face;
    LEDStrip_t   strip;
} Target_t;

/* ============================================================
 *  游戏全局状态
 * ============================================================ */
typedef struct {
    uint8_t  rune_mode;
    uint8_t  game_stage;
    uint8_t  flag_first_hit;
    uint8_t  flag_punish;
    uint32_t bonus_start;
    uint32_t activation_start;
    uint32_t success_start;
    uint32_t animation_last;
    uint8_t  big_rune_state;
} GameState_t;

/* ============================================================
 *  全局实例
 * ============================================================ */
extern Target_t    g_target[5];
extern GameState_t g_game;

/* ============================================================
 *  Target 层 API
 * ============================================================ */
void Target_InitAll(void);
void Target_SetFace(uint8_t idx, uint8_t mode);
void Target_SetLed(uint8_t idx, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void Target_ClearStrip(uint8_t idx);
void Target_LoadLED(uint8_t idx);
void Target_TransferAll(void);
uint8_t Target_ReadSensor(uint8_t idx);
void Target_LoadLEDPtr(LEDStrip_t *s);  // 直接接受指针的便捷版本
void Target_SequentialTest(void);       // 逐块灯板点亮测试
void Target_TransferNoDMA(uint8_t idx); // CPU直驱测试(绕过DMA)
void Target_FaceTest(void);             // 靶面继电器循环测试
void Target_TransferChangePart(void);        // 仅传输已变化的strip

#endif // TARGET_H