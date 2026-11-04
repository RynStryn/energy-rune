//
// Created by RynStryn on 2026/5/15.
//

#ifndef TC_UU_D_MKLK_PROTOCOL_H
#define TC_UU_D_MKLK_PROTOCOL_H

#include <stdint.h>
#include "color.h"

/*
 *  byte:  [7:6] center_idx  |  [5:4] fill_idx  |  [3:0] icon
 *
 *  填充颜色 fill_idx (2 bits):
 *    0 → BLACK   (关闭/无填充)
 *    1 → GREEN
 *    2 → RED
 *    3 → 保留
 *
 *  中心颜色 center_idx (2 bits):
 *    0 → BLACK   (关闭/无中心)
 *    1 → YELLOW
 *    2 → BLUE
 *    3 → 保留
 *
 *  图案 icon (4 bits):
 *     0 → 全黑 (清屏)
 *     1 → Needle_Up    (↑)
 *     2 → Needle_Down  (↓)
 *     3 → Needle_Left  (←)
 *     4 → Needle_Right (→)
 *     5 → Tower        (塔)
 *     6 → Buff         (X)
 *     7～15 预留
 * ============================================================ */

#define FILL_COLOR_TABLE_SIZE   4
#define CENTER_COLOR_TABLE_SIZE 4

extern const Color_t fill_color_table[FILL_COLOR_TABLE_SIZE];
extern const Color_t center_color_table[CENTER_COLOR_TABLE_SIZE];

typedef enum {
    ICON_NONE         = 0,
    ICON_NEEDLE_UP    = 1,
    ICON_NEEDLE_DOWN  = 2,
    ICON_NEEDLE_LEFT  = 3,
    ICON_NEEDLE_RIGHT = 4,
    ICON_TOWER        = 5,
    ICON_BUFF         = 6,
    ICON_WATER_FLOW   = 7,
} IconIndex_t;

typedef union {
    uint8_t raw;

    struct {
        uint8_t icon       : 4;  // bit[3:0]  图案索引
        uint8_t fill_idx   : 2;  // bit[5:4]  填充颜色索引
        uint8_t center_idx : 2;  // bit[7:6]  中心颜色索引
    }pack_t;
} CmdByte_t;

void Protocol_Init(void);
void Protocol_IRQHandler(void);
void Protocol_ProcessCmd(uint8_t rx_byte);

extern uint8_t protocol_rx_byte;

extern volatile uint8_t cmd_received_flag;
extern volatile uint8_t last_cmd_byte;

#if 0
/* ----- 红色系 (Fill=10) ----- */
#define CMD_MOVE_UP_NONE          0b00100001  // 0x21
#define CMD_MOVE_DOWN_NONE        0b00100010  // 0x22
#define CMD_ROTATE_LEFT_NONE      0b00100011  // 0x23
#define CMD_ROTATE_RIGHT_NONE     0b00100100  // 0x24

#define CMD_MOVE_UP_YELLOW        0b01100001  // 0x61
#define CMD_MOVE_DOWN_YELLOW      0b01100010  // 0x62
#define CMD_ROTATE_LEFT_YELLOW    0b01100011  // 0x63
#define CMD_ROTATE_RIGHT_YELLOW   0b01100100  // 0x64

#define CMD_MOVE_UP_BLUE          0b10100001  // 0xA1
#define CMD_MOVE_DOWN_BLUE        0b10100010  // 0xA2
#define CMD_ROTATE_LEFT_BLUE      0b10100011  // 0xA3
#define CMD_ROTATE_RIGHT_BLUE     0b10100100  // 0xA4

/* ----- 绿色系 (Fill=01) ----- */
#define CMD_MOVE_FWD_NONE         0b00010001  // 0x11
#define CMD_MOVE_BACK_NONE        0b00010010  // 0x12
#define CMD_MOVE_LEFT_MOVE_NONE   0b00010011  // 0x13
#define CMD_MOVE_RIGHT_MOVE_NONE  0b00010100  // 0x14

#define CMD_MOVE_FWD_YELLOW       0b01010001  // 0x51
#define CMD_MOVE_BACK_YELLOW      0b01010010  // 0x52
#define CMD_MOVE_LEFT_MOVE_YELLOW 0b01010011  // 0x53
#define CMD_MOVE_RIGHT_MOVE_YELLOW 0b01010100  // 0x54

#define CMD_MOVE_FWD_BLUE         0b10010001  // 0x91
#define CMD_MOVE_BACK_BLUE        0b10010010  // 0x92
#define CMD_MOVE_LEFT_MOVE_BLUE   0b10010011  // 0x93
#define CMD_MOVE_RIGHT_MOVE_BLUE  0b10010100  // 0x94

/* ----- 特殊图案 ----- */
#define CMD_BUFF_NONE    0b00100110  // 0x26
#define CMD_BUFF_YELLOW  0b01100110  // 0x66
#define CMD_BUFF_BLUE    0b10100110  // 0xA6

#define CMD_TOWER_NONE    0b00010101  // 0x15
#define CMD_TOWER_YELLOW  0b01010101  // 0x55
#define CMD_TOWER_BLUE    0b10010101  // 0x95

/* ----- 清屏 ----- */
#define CMD_CLEAR_ALL           0x00
#endif

#endif // TC_UU_D_MKLK_PROTOCOL_H
