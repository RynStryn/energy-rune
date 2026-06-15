//
// Created by 31466 on 2026/2/8.
//

#include "wireless.h"
#include "DMA_buffer_config.h"
#include "main.h"
#include "stm32h7xx_hal.h"

int Data_Process_Flag;

RingBuffer_t uart_ring_buffer;

extern UART_HandleTypeDef huart1;
void Start_Variable_UART(void) {
  uart_ring_buffer.pPut = 0;
  uart_ring_buffer.pGet = 0;

  // 启动接收
  // 模式必须是 DMA Circular
  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_ring_buffer.Buffer,
                                   RX_BUF_SIZE) != HAL_OK) {
//    Error_Handler();
  }

  // 不定长数据靠 IDLE 截断。HT 中断会打断 CPU
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size) {
  if (huart->Instance == USART1) {
    uart_ring_buffer.pPut = Size;

    // 2. 唤醒主任务解析数据
    Data_Process_Flag = 1;
  }
}