//
// Created by 31466 on 2026/2/8.
//

#ifndef ENERGYRUNE_H7_WIRELESS_H
#define ENERGYRUNE_H7_WIRELESS_H
#include <stdint.h>

#define RX_BUF_SIZE 1024  // 定义一个足够大的环形缓冲区
typedef struct {
    uint8_t  Buffer[RX_BUF_SIZE];
    uint32_t pPut; // 写入位置 (DMA会自动更新)
    uint32_t pGet; // 读取位置
} RingBuffer_t;
extern int Data_Process_Flag;
void Start_Variable_UART(void);
#endif //ENERGYRUNE_H7_WIRELESS_H