//
// Created by RynStryn on 2026/5/12.
//

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "protocol.h"
#include "graphics.h"
#include "led_panel.h"
#include "water_flow.h"
const Color_t fill_color_table[FILL_COLOR_TABLE_SIZE] = {
    {  0,   0,   0},  // 0: 关闭
    {  0, 255,   0},  // 1: 绿色
    {255,   0,   0},  // 2: 红色
    {  0,   0,   0},  // 3: 保留（默认为黑）
};

const Color_t center_color_table[CENTER_COLOR_TABLE_SIZE] = {
    {  0,   0,   0},  // 0: 关闭
    {255, 255,   0},  // 1: 黄色
    {  0,   0, 255},  // 2: 蓝色
    {  0,   0,   0},  // 3: 保留（默认为黑）
};

static void Cmd_Clear(void)
{
    Panel_Clear();
    Panel_load2led();
}

/* ---- 串口接收缓冲区 ---- */
uint8_t protocol_rx_byte;

/* ---- 中断级标志（volatile 防止编译器优化） ---- */
volatile uint8_t cmd_received_flag = 0;
volatile uint8_t last_cmd_byte     = 0;

/* ---- 协议初始化：开启串口中断接收 ---- */
void Protocol_Init(void)
{
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void Protocol_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        protocol_rx_byte = (uint8_t)USART_ReceiveData(USART1);
        last_cmd_byte = protocol_rx_byte;
        cmd_received_flag = 1;
    }
}


void Protocol_ProcessCmd(uint8_t rx_byte)
{
    CmdByte_t cmd;
   cmd.raw = rx_byte;

    /* 查颜色表（越界则默认黑色） */
    Color_t fill_color   = {0, 0, 0};
    Color_t center_color = {0, 0, 0};

    if (cmd.pack_t.fill_idx   < FILL_COLOR_TABLE_SIZE)
        fill_color   = fill_color_table[cmd.pack_t.fill_idx];
    if (cmd.pack_t.center_idx < CENTER_COLOR_TABLE_SIZE)
        center_color = center_color_table[cmd.pack_t.center_idx];

    //图案索引
    switch (cmd.pack_t.icon) {
        case ICON_NONE:
            Cmd_Clear();
            break;

        case ICON_NEEDLE_UP:
            Needle_Up_WithCenter(fill_color, center_color);
            break;

        case ICON_NEEDLE_DOWN:
            Needle_Down_WithCenter(fill_color, center_color);
            break;

        case ICON_NEEDLE_LEFT:
            Needle_Left_WithCenter(fill_color, center_color);
            break;

        case ICON_NEEDLE_RIGHT:
            Needle_Right_WithCenter(fill_color, center_color);
            break;

        case ICON_TOWER:
            Tower_WithCenter(fill_color, center_color);
            break;

        case ICON_BUFF:
            Buff_WithCenter(fill_color, center_color);
            break;

        case ICON_WATER_FLOW:
            WaterFlow_StartArrow(fill_color);
            break;

        default:

            break;
    }
}
