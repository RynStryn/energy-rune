#include "ws2812.h"
#include <string.h>
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#define BIT0_HIGH 3 // 22
#define BIT1_HIGH 8 // 58

#define FRAME_BITS (WS2812_NUM_LEDS * 24 + 240)

static uint16_t pwm_buffer[FRAME_BITS];

static void Color2Pwm(uint8_t color, uint16_t* pwm_buf)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        *pwm_buf++ = (color & (1 << i)) ? BIT1_HIGH : BIT0_HIGH;
    }
}

static void delay_us(uint32_t us)
{
    while (us--)
    {
        for (uint32_t i = 0; i < 42; i++)
        {
            __NOP();
        }
    }
}

void WS2812_Reset(void)
{
    TIM_DMACmd(TIM2, TIM_DMA_CC1, DISABLE);
    TIM_Cmd(TIM2, DISABLE);
    DMA_Cmd(DMA1_Stream5, DISABLE);

    TIM_SetCompare1(TIM2, 0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}

void WS2812_Init(void) { WS2812_Reset(); }

void WS2812_SendFrame(const uint8_t* grb_array)
{
    memset(pwm_buffer, 0, sizeof(pwm_buffer));

    uint16_t* p_buf = pwm_buffer;
    for (int i = 0; i < WS2812_NUM_LEDS; i++)
    {
        Color2Pwm(grb_array[0], p_buf);
        p_buf += 8;
        Color2Pwm(grb_array[1], p_buf);
        p_buf += 8;
        Color2Pwm(grb_array[2], p_buf);
        p_buf += 8;
        grb_array += 3;
    }

    DMA_Cmd(DMA1_Stream5, DISABLE);
    uint32_t timeout = 0;
    while ((DMA1_Stream5->CR & DMA_SxCR_EN) && (timeout < 10000))
    {
        timeout++;
    }

    DMA1_Stream5->M0AR = (uint32_t)pwm_buffer;
    DMA_SetCurrDataCounter(DMA1_Stream5, FRAME_BITS);

    DMA_Cmd(DMA1_Stream5, ENABLE);

    TIM_DMACmd(TIM2, TIM_DMA_CC1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);

    while (DMA_GetFlagStatus(DMA1_Stream5, DMA_FLAG_TCIF5) == RESET)
        ;
    DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);

    delay_us(500);
    WS2812_Reset();
}
