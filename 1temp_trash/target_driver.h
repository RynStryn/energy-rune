#ifndef __TARGET_DRIVER_H
#define __TARGET_DRIVER_H
#include <stdint.h>

// 初始化GPIO
void Target_Hardware_Init(void);

// 更新所有靶子状态
// 参数 pModes: 指向包含5个状态值的数组 (0:下, 1:上, 2:击中/完成)
void Target_Set_All(uint8_t *pModes);

// 读取所有传感器
// 参数 sensor_buf: 指向包含5个int的数组，用于存储读取结果
void Sensor_Read_All(int* sensor_buf);

#endif