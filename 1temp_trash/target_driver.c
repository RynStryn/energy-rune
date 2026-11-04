#include "target_driver.h"
#include <stdint.h>

#include "hardware_config.h"
#include "stm32h7xx_hal.h"

static void Target_Set_State(GPIO_TypeDef* port,
                             uint16_t pin_a,
                             uint16_t pin_b,
                             uint8_t mode) {
  switch (mode) {
    case 0:  // ResetBits -> WritePin(RESET)
      HAL_GPIO_WritePin(port, pin_a | pin_b, GPIO_PIN_RESET);
      break;
    case 1:  // SetBits -> WritePin(SET)
      HAL_GPIO_WritePin(port, pin_a | pin_b, GPIO_PIN_SET);
      break;
    case 2:
      HAL_GPIO_WritePin(port, pin_a, GPIO_PIN_SET);
      HAL_GPIO_WritePin(port, pin_b, GPIO_PIN_RESET);
      break;
    default:
      HAL_GPIO_WritePin(port, pin_a, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(port, pin_b, GPIO_PIN_SET);
      break;
  }
}
// ????? 10 ?????????? 5 ????????
#if 0
void Target_Hardware_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    // 1. ???????????????
    RCC_AHB1PeriphClockCmd(TARGET_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = T1_PIN_A | T1_PIN_B | T2_PIN_A | T2_PIN_B |
                                  T3_PIN_A | T3_PIN_B | T4_PIN_A | T4_PIN_B |
                                  T5_PIN_A | T5_PIN_B;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(TARGET_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(TARGET_PORT, 0xFFFF); // ??????

    // 2. ??????????????????? (PB/PC)
    RCC_AHB1PeriphClockCmd(SENSOR_1_RCC | SENSOR_4_RCC, ENABLE); 

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;

    // ????? PB3, PB4, PB5
    GPIO_InitStructure.GPIO_Pin = SENSOR_1_PIN | SENSOR_2_PIN | SENSOR_3_PIN;
    GPIO_Init(SENSOR_1_PORT, &GPIO_InitStructure);

    // ????? PC11, PC12
    GPIO_InitStructure.GPIO_Pin = SENSOR_4_PIN | SENSOR_5_PIN;
    GPIO_Init(SENSOR_4_PORT, &GPIO_InitStructure);
}
#endif

// ?? buffTask ????????????
void Target_Set_All(uint8_t* pModes) {
  Target_Set_State(T1_PORT, T1_PIN_A, T1_PIN_B, pModes[0]);
  Target_Set_State(T2_PORT, T2_PIN_A, T2_PIN_B, pModes[1]);
  Target_Set_State(T3_PORT, T3_PIN_A, T3_PIN_B, pModes[2]);
  Target_Set_State(T4_PORT, T4_PIN_A, T4_PIN_B, pModes[3]);
  Target_Set_State(T5_PORT, T5_PIN_A, T5_PIN_B, pModes[4]);
}
// TODO
//  ?? buffTask ??????????????
void Sensor_Read_All(int* sensor_buf) {
  sensor_buf[0] = HAL_GPIO_ReadPin(SENSOR_1_PORT, SENSOR_1_PIN);
  sensor_buf[1] = HAL_GPIO_ReadPin(SENSOR_2_PORT, SENSOR_2_PIN);
  sensor_buf[2] = HAL_GPIO_ReadPin(SENSOR_3_PORT, SENSOR_3_PIN);
  sensor_buf[3] = HAL_GPIO_ReadPin(SENSOR_4_PORT, SENSOR_4_PIN);
  sensor_buf[4] = HAL_GPIO_ReadPin(SENSOR_5_PORT, SENSOR_5_PIN);
}