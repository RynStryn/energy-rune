//
// Created by 31466 on 2026/2/8.
//

#ifndef ENERGYRUNE_H7_DMA_BUFFER_SETTING_H
#define ENERGYRUNE_H7_DMA_BUFFER_SETTING_H
#include "WS2812B.h"
#include "wireless.h"

// 必须 32 字节对齐以适配 D-Cache
//改动时记得检查mpu_config
// 17000 * 2 = 34000 Bytes (约 33KB)

#if defined(__GNUC__)
    /* arm-none-eabi-gcc / STM32CubeIDE */
uint16_t RGB_DMA_buffer4TIM3[3][WS28_SENDBUFF_SIZE] __attribute__((section(
    ".ARM.__at_0x30000000")));
//
uint16_t RGB_DMA_buffer4TIM4[2][WS28_SENDBUFF_SIZE] __attribute__((section(
    ".ARM.__at_0x30020000")));
RingBuffer_t uart_ring_buffer __attribute__((section(".ARM.__at_0x30020000")));
#else
uint16_t RGB_DMA_buffer4TIM3[3][WS28_SENDBUFF_SIZE] __attribute__((section(
    ".RAM_D2_SRAM1")));
//
uint16_t RGB_DMA_buffer4TIM4[2][WS28_SENDBUFF_SIZE] __attribute__((section(
    ".RAM_D2_SRAM2")));
RingBuffer_t uart_ring_buffer __attribute__((section(".RAM_D2_SRAM2")));
#endif

#endif //ENERGYRUNE_H7_DMA_BUFFER_SETTING_H
