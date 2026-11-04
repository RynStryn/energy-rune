#ifndef TARGET_H
#define TARGET_H

#include <stdint.h>
#include "stm32h7xx_hal.h"
#include "led_panel.h"
#include "map.h"

#define SENSOR_1_PIN GPIO_PIN_14  // PB14
#define SENSOR_1_PORT GPIOB

#define SENSOR_2_PIN GPIO_PIN_10  // PD10
#define SENSOR_2_PORT GPIOD

#define SENSOR_3_PIN GPIO_PIN_11  // PD11
#define SENSOR_3_PORT GPIOD

#define SENSOR_4_PIN GPIO_PIN_8  // PC8
#define SENSOR_4_PORT GPIOC

#define SENSOR_5_PIN GPIO_PIN_9  // PC9
#define SENSOR_5_PORT GPIOC




#define STRIP_LEDS_PER_LEAF  320
#define STRIP_ROWS            8
#define STRIP_COLS           40
#define DMA_BUFF_SIZE        (STRIP_LEDS_PER_LEAF * 24 * 2)  // 15360

// WS2812B 时序
#define TIMING_ONE   180
#define TIMING_ZERO   72




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




typedef struct {
    GPIO_TypeDef *port_a;
    uint16_t      pin_a;
    GPIO_TypeDef *port_b;
    uint16_t      pin_b;
    uint8_t       mode;     // 0=熄灭, 1=中心, 2=全亮, 3=圆周
} TargetFace_t;




typedef struct {
    //  硬件
    TIM_HandleTypeDef *htim;
    uint32_t           channel;
    volatile uint16_t *dma_buffer;   // [DMA_BUFF_SIZE]

    //  虚拟面板 (8×40) 
    Led_t     panel[STRIP_ROWS][STRIP_COLS];
    uint8_t   grb_buffer[STRIP_LEDS_PER_LEAF * 3];  // 960 字节

    //  坐标映射
    const uint16_t (*map)[STRIP_COLS];

    //  状态 
    LED_mode_type state;
    uint32_t      hit_timestamp;
    uint8_t       last_input;
    uint8_t       animation_frame;
    uint8_t       water_flow_active;
    uint8_t       panel_change;
} LEDStrip_t;


typedef struct {
    TargetFace_t face;
    LEDStrip_t   strip;
} Target_t;


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


extern Target_t    g_target[5];
extern GameState_t g_game;



#define HIT_LOG_SIZE 64
extern volatile uint8_t g_hit_log[HIT_LOG_SIZE];
extern volatile uint8_t g_hit_log_cnt;



void Target_InitAll(void);
void Target_RecordHit(uint8_t idx);
void Target_SetFace(uint8_t idx, uint8_t mode);
void Target_SetLed(uint8_t idx, uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void Target_ClearStrip(uint8_t idx);
void Target_LoadLED(uint8_t idx);
void Target_TransferAll(void);
uint8_t Target_ReadSensor(uint8_t idx);
void Target_LoadLEDPtr(LEDStrip_t *s);
void strip_SequentialTest(void);
void Target_TransferNoDMA(uint8_t idx);
void Target_FaceTest(void);
void Target_TransferChangePart(void);

#endif // TARGET_H