#ifndef ENERGYRUNE_H7_DMA_BUFFER_SETTING_H
#define ENERGYRUNE_H7_DMA_BUFFER_SETTING_H

// DMA 缓冲区声明已移至 0ws_ graph/target.c
// 此文件保留用于向后兼容，声明 uart_ring_buffer

#include "wireless.h"

#if defined(__GNUC__)
extern RingBuffer_t uart_ring_buffer;
#else
extern RingBuffer_t uart_ring_buffer;
#endif

#endif  // ENERGYRUNE_H7_DMA_BUFFER_SETTING_H