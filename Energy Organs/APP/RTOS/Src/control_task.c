#include "control_task.h"
//本程序作为主板做逻辑处理和按键检测并控制三片扇叶
//transfer5,作为第五块板使用tim3的ch2-PA7  单独使用uint16_t 数组传输
//transfer1,4分别为leaf1/4，tim5的ch1和ch4 =分别为pa0和pa3共用uint32_t 数组
//尝试解决PA7首个灯乱闪，未果2.25
//1板can对2板通信
//2.26完成超时检测
//3.8 1的pa0，2的pa3，1的pa3，
//a0_pb5；a3_pc12;2a3_pb3;a7_pc11;2a0_PB4   
//6.28 1的PA2进行对外接f1的当前扇叶告知
//
int time_tick = 0;
int led_time1 = 1;
int all_finish_time[5]={0,0,0,0,0};
int last_led_time1 = 24 ;

int time1 =0;
int turn_flag = 0;
int energe_mode = 0;
int energe_leaf = 0;
long long time_Big_energe=0;


int energe_leaf_all_rand_flag = 0;
leaf LED[5]={0,0,0,0,0};
int current_leaf;
uint8_t n=1;
uint8_t m;
uint8_t flag_start=0;//                                                                                                                                          
uint8_t count_leaf=0;//完成数
uint8_t flag_all_finish=0;
uint8_t finish_flag=0;
uint8_t rand_finish=0;
uint8_t ready_to_reset=0;

void LED_task(void)
{		
    if(time_tick%50==0)
	{m=0;
		switch(LED[m].mode)
		{
			case Waiting_hit:		
					LED_current(m);				//流水箭头对应数组填充
		
					side_off(m);					//侧面关
					LED_target(m);				//靶子图案		
////			LED_current_all_on(m);		//箭头流水全亮
////			side_on(m);
////			LED_circle(m);
					break;
			case hit_finish:
					LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);				//靶子全亮		//流水			//检测
				break;
			case LED_none:
										my_ws2812_set_all_off(m);
				break;
			case all_finish:			
				LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);	
				break;
		
		}	   
		transfer(m);
	}


	if(time_tick%50==20)
	{m=3;
	switch(LED[m].mode)
		{
            case Waiting_hit:		
					LED_current(m);				//流水箭头对应数组填充
		
					side_off(m);					//侧面关
					LED_target(m);				//靶子图案		
////			LED_current_all_on(m);		//箭头流水全亮
////			side_on(m);
////			LED_circle(m);
				break;
			case hit_finish:
					LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);				//靶子全亮		//流水			//检测
				break;
			case LED_none:
										my_ws2812_set_all_off(m);
				break;
			case all_finish:			
				LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);	
				break;
		}   
		transfer(m);
	
	}
	if(time_tick%50==2)
	{m=4;
	switch(LED[m].mode)
		{
        case Waiting_hit:		
					LED_current(m);				//流水箭头对应数组填充
		
					side_off(m);					//侧面关
					LED_target(m);				//靶子图案		
//			LED_current_all_on(m);		//箭头流水全亮
//			side_on(m);
//			LED_circle(m);
				break;
			case hit_finish:
					LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);				//靶子全亮		//流水			//检测
				break;
			case LED_none:
										my_ws2812_set_all_off(m);
				break;
			case all_finish:			
				LED_current_all_on(m);		//箭头流水全亮
					side_off(m);					//侧面亮
					LED_circle(m);	
				break;
		}	   
		transfer(m);
	
	}
    }
void control_task(void)
{	
	time_tick++;
	CAN1_Send(LED[1].mode,LED[2].mode,n,0,0);
if(flag_start==1)//测试用接收到后就开始
{	
LED_task();
}
	if(time_tick%50==0)
	{
		
		time1++;
	}
	if(RC_CtrlData.rc.s2 == 0)		//总开关
	{	
		
		if(flag_start==1)//开始随机种子
		{
		srand(time_tick);

		}
		

		if(LED[0].mode != Waiting_hit && LED[1].mode != Waiting_hit && LED[2].mode != Waiting_hit &&LED[3].mode != Waiting_hit && LED[4].mode != Waiting_hit&&rand_finish==0)
		{
            while(LED[n].mode!=LED_none&&LED[0].mode!= all_finish)//防重复+结束检测
            {

                n=rand()%5;//当前数组 
	
                if(LED[0].mode == hit_finish && LED[1].mode == hit_finish && LED[2].mode == hit_finish &&LED[3].mode == hit_finish && LED[4].mode == hit_finish)
                {
                    for(uint8_t i=0 ; i<5 ; i++)
                    {
                        LED[i].mode = all_finish;
                        count_reset=2000;
		
                    }
                    reset_time++;
                    break;
                }
            }
		}//开启下一个扇叶
	
		if(LED[n].mode==LED_none){
		LED[n].mode=Waiting_hit;	
					
		}

		
		
if(flag_start==0)
	
{
//	LED[0].mode=LED_none;
//	LED[1].mode=LED_none;
//	LED[2].mode=LED_none;
//	LED[3].mode=LED_none;
//	LED[4].mode=LED_none;
	LED_task();
}
		


key_input();			//过符
	if(time_tick%35==0)
	{
		led_time1++;
		if(led_time1>8) led_time1 = 1;
	}	
		

//	}
	if(time_tick%50==0)
	{
        uint8_t tx_buf[]={0x40,LED[0].mode,LED[1].mode,LED[2].mode,LED[3].mode,LED[4].mode};
        USART2_SendArray(tx_buf,6);
	}
	if(time_tick%10000000==0)time_tick=0;
}
}
int rc_input = 0;
int last_rc_input = 0;



void clean_time(void)
{
	led_time1 = 1;
	last_led_time1 = 24 ;

}	


void control_task_Init(void)
{
	
}
