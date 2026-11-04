#ifndef __PUBLIC_H
#define __PUBLIC_H

/**************ST HEAD*********************/

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_spi.h"

/*************MATH HEAD********************/
#include <arm_math.h>
#include <math.h>

// ����PI ֵ
#ifndef PI
#define PI 3.14159265358979f
#endif

// ���� �Ƕ�(��)ת���� ���ȵı���
#ifndef ANGLE_TO_RAD
#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
#endif

// ���� ���� ת���� �Ƕȵı���
#ifndef RAD_TO_ANGLE
#define RAD_TO_ANGLE 57.295779513082320876798154814105f
#endif

#define VAL_LIMIT(val, min, max) \
  if (val <= min) {              \
    val = min;                   \
  } else if (val >= max) {       \
    val = max;                   \
  }

/**************TOOL HEAD*******************/
#include <delay.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*************Algorithm********************/

/**************senior**********************/
#include "CanBus.h"
#include "senior.h"

/****************BSP**********************/
#include "BSP.h"
#include "can.h"
#include "led.h"
#include "timer.h"
#include "usart.h"
#include "usart_o.h"

/***************TASK*********************/
#include "CAN_motor.h"
#include "Motor.h"
#include "RC.h"
#include "pid.h"
#include "ws2812b.h"

/**************RTOS**********************/

#include "control_task.h"

#endif
