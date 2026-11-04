//
// Created by 31466 on 2026/2/9.
//

#include "can.h"

#if 0
void Motor_620_out(CAN_TypeDef *CANx,
                   int16_t      value_16bit1,
                   int16_t      value_16bit2,
                   int16_t      value_16bit3,
                   int16_t      value_16bit4)
{
    CanTxMsg Motor_620_CanTxMsg;
    Motor_620_CanTxMsg.StdId   = 0x200;
    Motor_620_CanTxMsg.IDE     = CAN_Id_Standard;
    Motor_620_CanTxMsg.RTR     = CAN_RTR_Data;
    Motor_620_CanTxMsg.DLC     = 0x08;
    Motor_620_CanTxMsg.Data[0] = (uint8_t) (value_16bit1 >> 8);
    Motor_620_CanTxMsg.Data[1] = (uint8_t) value_16bit1;
    Motor_620_CanTxMsg.Data[2] = (uint8_t) (value_16bit2 >> 8);
    Motor_620_CanTxMsg.Data[3] = (uint8_t) value_16bit2;
    Motor_620_CanTxMsg.Data[4] = (uint8_t) (value_16bit3 >> 8);
    Motor_620_CanTxMsg.Data[5] = (uint8_t) value_16bit3;
    Motor_620_CanTxMsg.Data[6] = (uint8_t) (value_16bit4 >> 8);
    Motor_620_CanTxMsg.Data[7] = (uint8_t) value_16bit4;
    CAN_Transmit(CANx, &Motor_620_CanTxMsg);
}
#endif
extern FDCAN_HandleTypeDef hfdcan1; // 或 hfdcan2，根据电路图确认
FDCAN_TxHeaderTypeDef      Motor_TxHeader;

void can_init(FDCAN_TxHeaderTypeDef TxHeader, int ID)
{
    TxHeader.Identifier          = ID;
    TxHeader.IdType              = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType         = FDCAN_DATA_FRAME;
    TxHeader.DataLength          = FDCAN_DLC_BYTES_8;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch       = FDCAN_BRS_OFF;
    TxHeader.FDFormat            = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker       = 0;
}

void can_sent(FDCAN_HandleTypeDef *  hfdcan,
              FDCAN_TxHeaderTypeDef *txHeader,
              int16_t                value_16bit1,
              int16_t                value_16bit2,
              int16_t                value_16bit3,
              int16_t                value_16bit4)
{
    uint8_t TxData[8];

    TxData[0] = (uint8_t) (value_16bit1 >> 8);
    TxData[1] = (uint8_t) value_16bit1;
    TxData[2] = (uint8_t) (value_16bit2 >> 8);
    TxData[3] = (uint8_t) value_16bit2;
    TxData[4] = (uint8_t) (value_16bit3 >> 8);
    TxData[5] = (uint8_t) value_16bit3;
    TxData[6] = (uint8_t) (value_16bit4 >> 8);
    TxData[7] = (uint8_t) value_16bit4;

    // 发送消息
    HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, txHeader, TxData);
}
