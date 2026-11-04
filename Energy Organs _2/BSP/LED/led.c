#include "led.h" 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
int Flag_Input_Circle4;
int Flag_Input_Circle3;
int Flag_Input_Circle2;
int Flag_Input_Circle1;

int Flag_Input_1 =0 ;
int Flag_Input_2 =0 ;
int Flag_Input_3 =0 ;
int Flag_Input_4 =0 ;
int Flag_Input_5 =0 ;
int Flag_Input_1_Last =0 ;
int Flag_Input_2_Last =0 ;
int Flag_Input_3_Last =0 ;
int Flag_Input_4_Last =0 ;
int Flag_Input_5_Last =0 ;

int flag2_return = 0;
int flag3_return = 0;
int flag4_return = 0;
int flag5_return = 0;

int Flag_Input_PA7;
int Flag_Input_PA6;
int Flag_Input_PC4;
int Flag_Input_PA7_Last;
int Flag_Input_PA6_Last;
int Flag_Input_PC4_Last;

int LED_mode[5] ;
int LED_circle_mode[5] ;
int Leaf_mode[5] ;

int cnt_Circle_1;
int cnt_Circle_2;
int cnt_Circle_3;
int cnt_Circle_4;

int flag_rc1;
int flag_rc11;

void key_input(void)
{
	if(Flag_Input_1 == 0 && LED[0].mode==Waiting_hit)
	{                                         
		Flag_Input_1=(!Sucker_PB_5);
	}
		if(Flag_Input_2 == 0 &&LED[1].mode==Waiting_hit)
	{
		Flag_Input_2=(!Sucker_PB_4);
	}
		if(Flag_Input_3 == 0&&LED[2].mode==Waiting_hit)
	{
			Flag_Input_3=(!Sucker_PB_3);
	}
		if(Flag_Input_4 == 0&&LED[3].mode==Waiting_hit)
	{
			Flag_Input_4=(!Sucker_PC_12);
	}
		if(Flag_Input_5 ==0&&LED[4].mode==Waiting_hit)
	{
			Flag_Input_5=(!Sucker_PC_11);
	}

//	if(RC_CtrlData.rc.ch0==1684&&LED_mode[energe_leaf1] == Waiting_hit)
//	{
//		flag_rc1 = 1;
//	}
//	if(RC_CtrlData.rc.ch0==1024&&flag_rc1 ==1&&LED_mode[energe_leaf1] == Waiting_hit)
//	{
//		flag_rc11 = 1;
//	}
//	if(LED_mode[energe_leaf1] == Waiting_hit&&flag_rc11 == 1)
//	{
//		Flag_Input_1 = 1;
//		flag_rc1 = 0;
//		flag_rc11 = 0;
//	}//遥控过符

//	Flag_Input_4=(!Sucker_PC_15);
//	Flag_Input_5=(!Sucker_PA_10);
	if( LED[0].mode== Waiting_hit && Flag_Input_1_Last == 0 && Flag_Input_1 ==1)
	{
		LED[0].mode = hit_finish;
	}
	if( LED[1].mode == Waiting_hit && Flag_Input_2_Last == 0 && Flag_Input_2 ==1)
	{
		LED[1].mode = hit_finish;
	}
	if(LED[2].mode == Waiting_hit && Flag_Input_3_Last == 0 && Flag_Input_3 ==1)
	{
		LED[2].mode = hit_finish;
	}
	if(LED[3].mode == Waiting_hit && Flag_Input_4_Last == 0 && Flag_Input_4 ==1)
	{
		LED[3].mode = hit_finish;
	}
	if(LED[4].mode == Waiting_hit && Flag_Input_5_Last == 0 && Flag_Input_5 ==1)
	{
		LED[4].mode = hit_finish;
	}

	Flag_Input_1_Last =Flag_Input_1;
	Flag_Input_2_Last =Flag_Input_2;
	Flag_Input_3_Last =Flag_Input_3;
	Flag_Input_4_Last =Flag_Input_4;
	Flag_Input_5_Last =Flag_Input_5;
	
	
}



//初始化PF9和PF10为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOF时钟

  //GPIOF9,F10初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	GPIO_SetBits(GPIOC,GPIO_Pin_1 | GPIO_Pin_2);//GPIOF9,F10设置高，灯灭

}


void Sucker_PC11_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruecture;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
	GPIO_InitStruecture.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruecture.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStruecture.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruecture.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruecture.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruecture);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_11);
	
	
}

void Sucker_PC12_Init(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStruecture;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	
	GPIO_InitStruecture.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruecture.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStruecture.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruecture.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruecture.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOC,&GPIO_InitStruecture);
	
	GPIO_ResetBits(GPIOC,GPIO_Pin_12);
	
	
}

void Sucker_PB5_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruecture;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	GPIO_InitStruecture.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruecture.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStruecture.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruecture.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruecture.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruecture);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);
}



void Sucker_PB4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruecture;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	GPIO_InitStruecture.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruecture.GPIO_Pin=GPIO_Pin_4;
	GPIO_InitStruecture.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruecture.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruecture.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruecture);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
}

void Sucker_PB3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruecture;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
	
	GPIO_InitStruecture.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruecture.GPIO_Pin=GPIO_Pin_3;
	GPIO_InitStruecture.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStruecture.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStruecture.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruecture);
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);
}


