#include "ws2812.h"
#include <string.h>
#include "stm32h7xx_hal.h"

// WS2812B PWM 占空比 (TIM period = 299, 240MHz TIM clock)
#define BIT0_HIGH 72
#define BIT1_HIGH 180

static void Color2Pwm(uint8_t color, volatile uint16_t* pwm_buf) {
  for (int8_t i = 7; i >= 0; i--) {
    *pwm_buf++ = (color & (1 << i)) ? BIT1_HIGH : BIT0_HIGH;
  }
}

void WS2812_Init(TIM_HandleTypeDef* htim, uint32_t channel) {
  (void)htim;
  (void)channel;
}

void WS2812_SendFrame(TIM_HandleTypeDef* htim,
                      uint32_t channel,
                      volatile uint16_t* dma_buffer,
                      const uint8_t* grb_array,
                      uint16_t num_leds) {
  uint16_t frame_bits = num_leds * 24;
  memset((void*)dma_buffer, 0, frame_bits * sizeof(uint16_t));

  volatile uint16_t* p_buf = dma_buffer;
  for (uint16_t i = 0; i < num_leds; i++) {
    Color2Pwm(grb_array[0], p_buf);
    p_buf += 8;
    Color2Pwm(grb_array[1], p_buf);
    p_buf += 8;
    Color2Pwm(grb_array[2], p_buf);
    p_buf += 8;
    grb_array += 3;
  }

  HAL_TIM_PWM_Start_DMA(htim, channel, (uint32_t*)dma_buffer, frame_bits);
}