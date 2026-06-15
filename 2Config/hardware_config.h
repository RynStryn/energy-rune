#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

#include "stm32h7xx_hal.h"

// ==========================================
// 1. ??????????
// ==========================================

// ???1????
#define T1_PIN_A GPIO_PIN_0  // PE0
#define T1_PIN_B GPIO_PIN_1  // PE1
#define T1_PORT GPIOE

// ???2????
#define T2_PIN_A GPIO_PIN_7  // PE7
#define T2_PIN_B GPIO_PIN_8  // PE8
#define T2_PORT GPIOE

// ???3????
#define T3_PIN_A GPIO_PIN_7  // PB7
#define T3_PIN_B GPIO_PIN_8  // PB8
#define T3_PORT GPIOB

// ???4????
#define T4_PIN_A GPIO_PIN_9  // PB9
#define T4_PIN_B GPIO_PIN_0  // PD0
#define T4_PORT GPIOB

// ???5????
#define T5_PIN_A GPIO_PIN_1  // PD1
#define T5_PIN_B GPIO_PIN_2  // PD2
#define T5_PORT GPIOD

// ==========================================
// 2. ??????????????
// ==========================================
#define SENSOR_1_PIN GPIO_PIN_14  // PB14
#define SENSOR_1_PORT GPIOB

#define SENSOR_2_PIN GPIO_PIN_10  // PD10
#define SENSOR_2_PORT GPIOD

#define SENSOR_3_PIN GPIO_PIN_11  // PD11
#define SENSOR_3_PORT GPIOD

#define SENSOR_4_PIN GPIO_PIN_8  // PC8
#define SENSOR_4_PORT GPIOC

#define SENSOR_5_PIN GPIO_PIN_9  // PC9
#define SENSOR_5_PORT GPIOC

// ==========================================
// 3. ????????????
// ==========================================

// USART1
#define USART1_TX_PIN GPIO_PIN_9   // PA9
#define USART1_RX_PIN GPIO_PIN_10  // PA10
#define USART1_PORT GPIOA

// FDCAN1
#define FDCAN1_RX_PIN GPIO_PIN_11  // PA11
#define FDCAN1_TX_PIN GPIO_PIN_12  // PA12
#define FDCAN1_PORT GPIOA

// TIMers
#define TIM1_CH1_PIN GPIO_PIN_9  // PE9
#define TIM1_CH1_PORT GPIOE

#define TIM2_CH2_PIN GPIO_PIN_1  // PA1
#define TIM2_CH2_PORT GPIOA

#define TIM3_CH3_PIN GPIO_PIN_0  // PB0
#define TIM3_CH3_PORT GPIOB

#define TIM4_CH1_PIN GPIO_PIN_12  // PD12
#define TIM4_CH1_PORT GPIOD

#define TIM5_CH1_PIN GPIO_PIN_0  // PA0
#define TIM5_CH1_PORT GPIOA

#endif