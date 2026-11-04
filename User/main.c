#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Key.h"

uint8_t KeyNum;
typedef struct
{uint8_t mode;//0为未击打，1为待击打，2为完成
}leaf;
leaf LED[5]={0,0,0,0,0};
int main(void)
{

	//OLED_Init();

	Key_Init();
	Serial_Init();
	
	while (1)
	{
if (packet_ready)
{	 
	 LED[0].mode=packet[1];//B 98
	 LED[1].mode=packet[2];//B 76
	 LED[2].mode=packet[3];//B 54
	 LED[3].mode=packet[4];//A 76
	 LED[4].mode=packet[5];//B 54
    packet_ready = 0;    
}

switch(LED[0].mode)
	{case 0:
		GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_8);
		break;
	case 1:
		GPIO_SetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_8);
	break;
	case 2:
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	break;
	default:
			GPIO_SetBits(GPIOB,GPIO_Pin_9);
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
		break;
	
}
switch(LED[1].mode)
	{case 0:
	GPIO_ResetBits(GPIOB,GPIO_Pin_7|GPIO_Pin_6);
		break;
	case 1:
		GPIO_SetBits(GPIOB,GPIO_Pin_7|GPIO_Pin_6);
	break;
	case 2:
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
		GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	break;
	default:
			GPIO_SetBits(GPIOB,GPIO_Pin_7);
		GPIO_ResetBits(GPIOB,GPIO_Pin_6);
		break;
	
}
	switch(LED[2].mode)
	{case 0:
GPIO_ResetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
		break;
	case 1:
		GPIO_SetBits(GPIOB,GPIO_Pin_5);
			GPIO_SetBits(GPIOA,GPIO_Pin_12);
	break;
	case 2:
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
		GPIO_ResetBits(GPIOB,GPIO_Pin_5);
	break;
	default:
			GPIO_SetBits(GPIOB,GPIO_Pin_5);
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
		break;
	
}
	switch(LED[3].mode)
	{case 0:
GPIO_ResetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_6);
		break;
	case 1:
		GPIO_SetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_6);
	break;
	case 2:
		GPIO_SetBits(GPIOA,GPIO_Pin_6);
		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
	break;
	default:
			GPIO_SetBits(GPIOA,GPIO_Pin_7);
		GPIO_ResetBits(GPIOA,GPIO_Pin_6);
		break;
	
}
	switch(LED[4].mode)
	{case 0:
GPIO_ResetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_4);
		break;
	case 1:
		GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_4);
	break;
	case 2:
		GPIO_SetBits(GPIOA,GPIO_Pin_5);
		GPIO_ResetBits(GPIOA,GPIO_Pin_4);
	break;
	default:
			GPIO_SetBits(GPIOA,GPIO_Pin_4);
		GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		break;
	
}
		
	}
}
