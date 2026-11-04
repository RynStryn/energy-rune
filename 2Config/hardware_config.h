#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

#include "stm32h7xx_hal.h"

// ==========================================
// 1. 靶子驱动引脚 (GPIOE)
// ==========================================
#define TARGET_PORT         GPIOE

// HAL 库引脚定义必须使用 GPIO_PIN_x
#define T1_PIN_A            GPIO_PIN_0
#define T1_PIN_B            GPIO_PIN_1
#define T2_PIN_A            GPIO_PIN_2
#define T2_PIN_B            GPIO_PIN_3
#define T3_PIN_A            GPIO_PIN_4
#define T3_PIN_B            GPIO_PIN_5
#define T4_PIN_A            GPIO_PIN_6
#define T4_PIN_B            GPIO_PIN_7
#define T5_PIN_A            GPIO_PIN_8
#define T5_PIN_B            GPIO_PIN_9

// ==========================================
// 2. 传感器输入
// ==========================================
#define SENSOR_1_PIN        GPIO_PIN_5  // PB5
#define SENSOR_1_PORT       GPIOB

#define SENSOR_2_PIN        GPIO_PIN_4  // PB4
#define SENSOR_2_PORT       GPIOB

#define SENSOR_3_PIN        GPIO_PIN_3  // PB3
#define SENSOR_3_PORT       GPIOB

#define SENSOR_4_PIN        GPIO_PIN_12 // PC12
#define SENSOR_4_PORT       GPIOC

#define SENSOR_5_PIN        GPIO_PIN_11 // PC11
#define SENSOR_5_PORT       GPIOC

#endif