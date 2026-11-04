//
// Created by 31466 on 2026/2/9.
//

#ifndef ENERGYRUNE_H7_CAN_H
#define ENERGYRUNE_H7_CAN_H
#include "stm32h7xx_hal.h"
void can_init(FDCAN_TxHeaderTypeDef TxHeader,int ID);
void can_sent(FDCAN_HandleTypeDef *  hfdcan,
              FDCAN_TxHeaderTypeDef *txHeader,
              int16_t                value_16bit1,
              int16_t                value_16bit2,
              int16_t                value_16bit3,
              int16_t                value_16bit4);
#endif //ENERGYRUNE_H7_CAN_H