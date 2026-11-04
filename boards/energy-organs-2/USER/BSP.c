#include "BSP.h"

void BSP_Init(void)
{


NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2


	//	
#if EN_USART1
	usart1_init(100000);	//��ʼ�����ڲ�����Ϊ115200
#endif
#if EN_USART2
	usart2_init(115200);	//��ʼ�����ڲ�����Ϊ115200
#endif
#if EN_USART3
	usart3_init(921600);	//��ʼ�����ڲ�����Ϊ115200
#endif
#if EN_UART4
	uart4_init(115200);	//��ʼ�����ڲ�����Ϊ115200
#endif
#if EN_UART5
	uart5_init(115200);	//��ʼ�����ڲ�����Ϊ115200
#endif
#if EN_USART6
	usart6_init();	//��ʼ�����ڲ�����Ϊ115200
#endif



	Sucker_PC12_Init();
	Sucker_PC11_Init();
	Sucker_PB5_Init();
	Sucker_PB4_Init();
	Sucker_PB3_Init();
	

#if EN_CAN1 

CAN1_Mode_Init(CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������100Kbps   42M	/��6+7+1��/30==1Mps
#endif
#if EN_CAN2
CAN2_Mode_Init(CAN_BS2_4tq,CAN_BS1_9tq,3,CAN_Mode_Normal);//CAN��ʼ������ģʽ,������100Kbps   42M/��6+7+1��/30==1Mps
#endif
	
	#if EN_TIM1
		TIM1_Configuration();
	#endif
	#if EN_TIM2
		TIM2_Configuration();
	#endif
	#if EN_TIM3
		TIM3_Configuration();
	#endif
	#if EN_TIM4
		TIM4_Configuration();
	#endif
	#if EN_TIM5
		TIM5_Configuration();
	#endif
	#if EN_TIM6
		TIM6_Configuration();
	#endif
	#if EN_TIM7
		TIM7_Configuration();
	#endif
	#if EN_TIM8
		TIM8_Configuration();
	#endif
	
}
	

	
	









