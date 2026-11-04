//
// Created by 31466 on 2026/2/8.
//

#include "wireless.h"
#include "stm32h7xx_hal.h"
#include "DMA_buffer_config.h"
#include "main.h"
int flag_data_process;
extern UART_HandleTypeDef huart1;
void Start_Variable_UART(void)
{
    // 初始化指针
    uart_ring_buffer.pDMA = 0;
    uart_ring_buffer.pCPU = 0;

    // 启动接收
    // 关键点：UART_RECEIVER_TIMEOUT disable (0) 或者设个值防死锁
    // 模式必须是 DMA Circular
    if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart_ring_buffer.Buffer, RX_BUF_SIZE) != HAL_OK)
    {
        Error_Handler();
    }

    //不定长数据靠 IDLE 截断。HT 中断会打断 CPU
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        // 1. 更新 Head 指针
        // Size 是 DMA 传输剩下还要传多少？还是当前传了多少？
        // 在 ReceiveToIdle_DMA 中，Size 参数 = 接收到的总字节数 (相对于 buffer 开头)
        // 注意：如果是 Circular 模式，Size 会在 Buffer 结尾归零重计吗？
        // HAL 库的一个坑：在 Circular 模式下，Size 指示的是 "Buffer 中数据的当前末尾索引"

        uart_ring_buffer.pDMA = Size;

        // 2. 唤醒主任务处理数据
        // 不要在这里做复杂解析，置个标志位或者释放信号量
        flag_data_process = 1;
    }
}