#ifndef TC_UU_D_MKLK_WS2812_H
#define TC_UU_D_MKLK_WS2812_H

#include <stdint.h>
#include "stm32h7xx_hal.h"

// 单通道 WS2812 驱动 (供非靶子系统的独立灯带使用)
#define WS2812_NUM_LEDS 320

void WS2812_Init(TIM_HandleTypeDef* htim, uint32_t channel);
void WS2812_SendFrame(TIM_HandleTypeDef* htim,
                      uint32_t channel,
                      volatile uint16_t* dma_buffer,
                      const uint8_t* grb_array,
                      uint16_t num_leds);

#endif  // TC_UU_D_MKLK_WS2812_H