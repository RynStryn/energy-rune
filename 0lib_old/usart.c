#include "usart.h"

/*****************************************VARIABLES*******************************************************/
/*********************************************************************************************************/                                             
#if EN_UART5_DMA_SECOND_FIFO == 1	
uint8_t _UART5_DMA_RX_BUF[2][BSP_UART5_DMA_RX_BUF_LEN];
#else
uint8_t _UART5_DMA_RX_BUF[100];
#endif

	static uint8_t _USART1_DMA_RX_BUF[2][BSP_USART1_DMA_RX_BUF_LEN];//˫���������
	static uint8_t _USART2_DMA_RX_BUF[2][BSP_USART2_DMA_RX_BUF_LEN];//˫���������
	static uint8_t _USART3_RX_BUF[BSP_USART2_DMA_RX_BUF_LEN];//ch100�����������
	static uint8_t _UART4_DMA_RX_BUF[UART4_RX_BUF_LENGTH];	
	static uint8_t _USART6_DMA_RX_BUF[BSP_USART6_RX_BUF_LENGTH];

//static uint8_t USART1_DMA_TX_BUF[USART1_TX_BUF_LENGTH];//ע�Ͳ���
//static uint8_t USART2_DMA_TX_BUF[USART2_TX_BUF_LENGTH];
//static uint8_t USART3_DMA_TX_BUF[USART3_TX_BUF_LENGTH];
	static uint8_t UART4_DMA_TX_BUF[UART4_TX_BUF_LENGTH];
	static uint8_t UART5_DMA_TX_BUF[UART5_TX_BUF_LENGTH];
//static uint8_t USART6_DMA_TX_BUF[USART6_TX_BUF_LENGTH];
/*********************************************************************************************************/
/*********************************************************************************************************/
#if 0
#pragma import(__use_no_semihosting)  
/**
************************************************************************************************************************
* @Name     : fputc/_sys_exit
* @brief    : �������´���,֧��printf����,������Ҫѡ��use MicroLIB
* @param    : ch
* @param    : FILE *f
* @retval   : void
* @Note     : �������´���,֧��printf����,������Ҫѡ��use MicroLIB
************************************************************************************************************************
**/ 
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2 ->DR = (u8) ch;      
	return ch;
}
#endif

#if EN_USART1
void usart1_init(uint32_t baud_rate)
{
		GPIO_InitTypeDef gpio;
		USART_InitTypeDef usart;
		NVIC_InitTypeDef nvic;
		DMA_InitTypeDef dma;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
	
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_7;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &gpio);
    
    USART_DeInit(USART1);
    USART_StructInit(&usart);
    usart.USART_BaudRate = baud_rate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_Even;
    usart.USART_Mode = USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart);
    
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    DMA_DeInit(DMA2_Stream2);
    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_4;
    dma.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);
    dma.DMA_Memory0BaseAddr = (uint32_t)&_USART1_DMA_RX_BUF[0][0];
    dma.DMA_DIR = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize = sizeof(_USART1_DMA_RX_BUF)/2;
    dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma.DMA_Mode = DMA_Mode_Circular;
    dma.DMA_Priority = DMA_Priority_Medium;
    dma.DMA_FIFOMode = DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream2, &dma);
    
    DMA_DoubleBufferModeConfig(DMA2_Stream2, (uint32_t)&_USART1_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
    DMA_DoubleBufferModeCmd(DMA2_Stream2, ENABLE);
    DMA_Cmd(DMA2_Stream2, ENABLE);
    
		nvic.NVIC_IRQChannel = USART1_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 0;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 0;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	

		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
		USART_Cmd(USART1, ENABLE);

}

//���ڽ����жϷ�����
void USART1_IRQHandler(void)
{
	static uint32_t this_time_rx_len = 0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		(void)USART1->SR;
		(void)USART1->DR;
		if(DMA_GetCurrentMemoryTarget(DMA2_Stream2) == 0)
		{
			DMA_Cmd(DMA2_Stream2, DISABLE);
			DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);	
			this_time_rx_len = BSP_USART1_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream2);
			DMA2_Stream2->NDTR = (uint16_t)BSP_USART1_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA2_Stream2->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA2_Stream2, ENABLE);
			if(this_time_rx_len == RC_FRAME_LENGTH)
			{
				USART1_Data_Receive_Process_0
			}
					

		}
		
		else 
		{
			DMA_Cmd(DMA2_Stream2, DISABLE);
			DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);
			this_time_rx_len = BSP_USART1_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA2_Stream2);
			DMA2_Stream2->NDTR = (uint16_t)BSP_USART1_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA2_Stream2->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA2_Stream2, ENABLE);
			if(this_time_rx_len == RC_FRAME_LENGTH)
			{
				USART1_Data_Receive_Process_1
			}
		}
	}       
}
#endif




#if EN_USART2
void USART2_Tx_Init(uint32_t baudrate) {
     GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    // 1. ʹ��GPIOA��USART2ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 2. ����PA2ΪUSART2_TX���ù���
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

    // 3. ��ʼ��PA2
    gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    // 4. USART2��ʼ����������
    USART_StructInit(&usart);
    usart.USART_BaudRate = baudrate;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Tx;  // ֻ������
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    // 5. ʹ��USART2
    USART_Cmd(USART2, ENABLE);
	}

//���ڽ����жϷ�����
void USART2_IRQHandler(void)
{
	static uint32_t this_time_rx_len2 = 0;
if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		(void)USART2->SR;
		(void)USART2->DR;

			DMA_Cmd(DMA1_Stream5, DISABLE);
			DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);	
			this_time_rx_len2 = BSP_USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			DMA1_Stream5->NDTR = (uint16_t)BSP_USART2_DMA_RX_BUF_LEN;     //relocate the dma memory pointer to the beginning position
			DMA1_Stream5->CR |= (uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 1
			DMA_Cmd(DMA1_Stream5, ENABLE);

					USART2_Data_Receive_Process_0
		}
		
		else 
		{
			DMA_Cmd(DMA1_Stream5, DISABLE);
			DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);
			this_time_rx_len2 = BSP_USART2_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
			DMA1_Stream5->NDTR = (uint16_t)BSP_USART2_DMA_RX_BUF_LEN;      //relocate the dma memory pointer to the beginning position
			DMA1_Stream5->CR &= ~(uint32_t)(DMA_SxCR_CT);                  //enable the current selected memory is Memory 0
			DMA_Cmd(DMA1_Stream5, ENABLE);

					USART2_Data_Receive_Process_1

		}
	}       
void USART2_SendChar(uint8_t c) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); // �ȴ����ͻ�������
    USART_SendData(USART2, c);
}
void USART2_SendByte(uint8_t data) {
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, data);
}
void USART2_SendString(const char *str) {
    while (*str) {
        USART2_SendChar((uint8_t)(*str++));
    }
	}
void USART2_SendArray(uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        USART2_SendByte(data[i]);
    }
}

	#endif



#if EN_USART3
void usart3_init(u32 bound)
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

  /* Peripheral Clock Enable -------------------------------------------------*/
  /* Enable GPIO clock */
  RCC_AHB1PeriphClockCmd(USART_CH100_TX_GPIO_CLK | USART_CH100_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USART_CH100_CLK_INIT(USART_CH100_CLK, ENABLE);
  
  /* Enable the DMA clock */
  RCC_AHB1PeriphClockCmd(USART_CH100_DMAx_CLK, ENABLE);
  
  /* USART_CH100 GPIO configuration -----------------------------------------------*/ 
  /* Connect USART pins to AF7 */
  GPIO_PinAFConfig(USART_CH100_TX_GPIO_PORT, USART_CH100_TX_SOURCE, USART_CH100_TX_AF);
  GPIO_PinAFConfig(USART_CH100_RX_GPIO_PORT, USART_CH100_RX_SOURCE, USART_CH100_RX_AF);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
  GPIO_InitStructure.GPIO_Pin = USART_CH100_TX_PIN;
  GPIO_Init(USART_CH100_TX_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = USART_CH100_RX_PIN;
  GPIO_Init(USART_CH100_RX_GPIO_PORT, &GPIO_InitStructure);
 
 
//		NVIC_InitStructure.NVIC_IRQChannel = USART_CH100_IRQn;

//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//


//    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;//

//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ


//	NVIC_Init(&NVIC_InitStructure);

  /* Enable the USART OverSampling by 8 */
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  /* When using Parity the word length must be configured to 9 bits */
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART_CH100, &USART_InitStructure);

	/*ʹ�ܿ���֡�ж�*/
   USART_ITConfig(USART_CH100,USART_IT_IDLE,ENABLE);
	 USART_ClearFlag(USART_CH100,USART_FLAG_TC|USART_FLAG_IDLE);
  /* Configure DMA controller to manage USART TX and RX DMA request ----------*/ 
   
  /* Configure DMA Initialization Structure */
  DMA_InitStructure.DMA_BufferSize =  BSP_USART1_DMA_RX_BUF_LEN;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_PeripheralBaseAddr =(uint32_t)rdata;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  /* Configure RX DMA */
  DMA_InitStructure.DMA_Channel = USART_CH100_TX_DMA_CHANNEL ;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory ;
  DMA_InitStructure.DMA_Memory0BaseAddr =(uint32_t)(&(USART_CH100->DR)) ; 
  DMA_Init(USART_CH100_TX_DMA_STREAM,&DMA_InitStructure);
   /* Enable DMA USART RX Stream */
  DMA_Cmd(USART_CH100_TX_DMA_STREAM,ENABLE);
  /* Enable USART DMA RX Requsts */
  USART_DMACmd(USART_CH100, USART_DMAReq_Tx, ENABLE);
  /* Enable USART */
  USART_Cmd(USART_CH100, ENABLE);
}


void USART_CH100_IRQHandler(void)
{
	if(USART_GetITStatus(USART_CH100, USART_IT_IDLE)!= RESET)//
	{
		USART_ReceiveData(USART_CH100); //һ��Ҫ��һ�Σ���Ȼ���ܻᶪ��һ���ֽڣ�ԭ��δ֪
		USART_ClearITPendingBit(USART_CH100,USART_IT_IDLE);//����жϱ�־λ
		DMA_Cmd(USART_CH100_RX_DMA_STREAM,DISABLE);  
		USART_DMACmd(USART_CH100, USART_DMAReq_Rx, DISABLE);

		USART3_Data_Receive_Process
		
		
		USART_DMACmd(USART_CH100, USART_DMAReq_Rx, ENABLE);
		DMA_Cmd(USART_CH100_RX_DMA_STREAM,ENABLE);//������λ�󣬵�ַָ����0
	}
}
#endif

#if EN_UART4
	/*-----UART4_TX-----PC10-----*/
/*-----UART4_RX-----PC11-----*/
	void uart4_init(u32 bound)
{
  USART_InitTypeDef uart4;
  GPIO_InitTypeDef  gpio;
  NVIC_InitTypeDef  nvic;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  gpio.GPIO_Mode = GPIO_Mode_AF;
  gpio.GPIO_OType = GPIO_OType_PP;
  gpio.GPIO_Speed = GPIO_Speed_100MHz;
  gpio.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC,&gpio);

  uart4.USART_BaudRate = bound;          // speed 10byte/ms
  uart4.USART_WordLength = USART_WordLength_8b;
  uart4.USART_StopBits = USART_StopBits_1;
  uart4.USART_Parity = USART_Parity_No;
  uart4.USART_Mode = USART_Mode_Tx|USART_Mode_Rx;
  uart4.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(UART4,&uart4);

  USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);


  DMA_InitTypeDef dma;
  DMA_DeInit(DMA1_Stream2);
  DMA_StructInit(&dma);
  dma.DMA_Channel = DMA_Channel_4;
  dma.DMA_PeripheralBaseAddr		= (uint32_t)(&UART4->DR);
  dma.DMA_Memory0BaseAddr   		= (uint32_t)&_UART4_DMA_RX_BUF;
  dma.DMA_DIR 					= DMA_DIR_PeripheralToMemory;
  dma.DMA_BufferSize				= UART4_RX_BUF_LENGTH;//sizeof(USART1_DMA_RX_BUF);
  dma.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
  dma.DMA_MemoryInc 				= DMA_MemoryInc_Enable;
  dma.DMA_PeripheralDataSize 		= DMA_PeripheralDataSize_Byte;
  dma.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
  dma.DMA_Mode 					= DMA_Mode_Normal;
  dma.DMA_Priority 				= DMA_Priority_Medium;
  dma.DMA_FIFOMode 				= DMA_FIFOMode_Disable;
  dma.DMA_FIFOThreshold 			= DMA_FIFOThreshold_1QuarterFull;
  dma.DMA_MemoryBurst 			= DMA_MemoryBurst_Single;
  dma.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream2, &dma);
  DMA_Cmd(DMA1_Stream2, ENABLE);

  nvic.NVIC_IRQChannel = UART4_IRQn;
  nvic.NVIC_IRQChannelPreemptionPriority = 3;
  nvic.NVIC_IRQChannelSubPriority =3;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);

  USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);

  DMA_Cmd(DMA1_Stream4, DISABLE);                           // ��DMAͨ��
  DMA_DeInit(DMA1_Stream4);
  while(DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}
  dma.DMA_Channel = DMA_Channel_4;
  dma.DMA_PeripheralBaseAddr	= (uint32_t)(&UART4->DR);
  dma.DMA_Memory0BaseAddr   	= (uint32_t)&UART4_DMA_TX_BUF[0];
  dma.DMA_DIR 			   				 = DMA_DIR_MemoryToPeripheral;
  dma.DMA_BufferSize					= 0;//sizeof(USART1_DMA_TX_BUF);
  dma.DMA_PeripheralInc 			= DMA_PeripheralInc_Disable;
  dma.DMA_MemoryInc 					= DMA_MemoryInc_Enable;
  dma.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
  dma.DMA_MemoryDataSize 			= DMA_MemoryDataSize_Byte;
  dma.DMA_Mode 								= DMA_Mode_Normal;
  dma.DMA_Priority 						= DMA_Priority_Medium;
  dma.DMA_FIFOMode 						= DMA_FIFOMode_Disable;
  dma.DMA_FIFOThreshold 			= DMA_FIFOThreshold_Full;
  dma.DMA_MemoryBurst 				= DMA_MemoryBurst_Single;
  dma.DMA_PeripheralBurst 		= DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream4,&dma);

//	DMA_Cmd(DMA1_Stream3, DISABLE);                           // ��DMAͨ��
  nvic.NVIC_IRQChannel = DMA1_Stream4_IRQn;   // ����DMAͨ�����ж�����
  nvic.NVIC_IRQChannelPreemptionPriority = 3;     // ���ȼ�����
  nvic.NVIC_IRQChannelSubPriority = 3;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&nvic);
  DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);

  USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);
  USART_Cmd(UART4,ENABLE);

}

void UART4_IRQHandler(void)
{
  uint8_t length=0;
  if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET)    //�����ж�
    {
      (void)UART4->SR;
      (void)UART4->DR;
      DMA_Cmd(DMA1_Stream2, DISABLE);
      DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2);
      length = UART4_RX_BUF_LENGTH - DMA_GetCurrDataCounter(DMA1_Stream2);

			UART4_Data_Receive_Process
      DMA_SetCurrDataCounter(DMA1_Stream2,UART4_RX_BUF_LENGTH);
      DMA_Cmd(DMA1_Stream2, ENABLE);

    }
}


void Uart4DmaSendDataProc(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)

{
//    DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA����
//    while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}  //ȷ��DMA���Ա�����
  DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����
  DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA����
}



void DMA1_Stream4_IRQHandler(void)
{
  //�����־
  if(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4)!=RESET)//�ȴ�DMA1_Steam3�������
    {
      DMA_Cmd(DMA1_Stream4, DISABLE);                      //�ر�DMA����
      DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);//���DMA1_Steam3������ɱ�־
    }
}


//���͵��ֽ�
void Uart4SendByteInfoProc(u8 nSendInfo)
{
  u8 *pBuf = NULL;
  //ָ���ͻ�����
  pBuf = UART4_DMA_TX_BUF;
  *pBuf++ = nSendInfo;

  Uart4DmaSendDataProc(DMA1_Stream4,1); //��ʼһ��DMA���䣡

}

//���Ͷ��ֽ�

void Uart4SendBytesInfoProc(u8* pSendInfo, u16 nSendCount)
{
  u16 i = 0;
  u8 *pBuf = NULL;
  //ָ���ͻ�����
  pBuf = UART4_DMA_TX_BUF;
  for (i=0; i<nSendCount; i++)
    {
      *(pBuf+i) = pSendInfo[i];
    }

  //DMA���ͷ�ʽ

  Uart4DmaSendDataProc(DMA1_Stream4,nSendCount); //��ʼһ��DMA���䣡
}

#endif

			

#if EN_UART5
void uart5_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  //IO��ʼ��
  GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType  =   GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_12;
  GPIO_InitStructure.GPIO_PuPd   =   GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Mode   =   GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType  =   GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Pin    =   GPIO_Pin_2;
  GPIO_InitStructure.GPIO_PuPd   =   GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed  =   GPIO_Speed_100MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC,GPIO_PinSource12, GPIO_AF_UART5);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource2, GPIO_AF_UART5);
  //����5��ʼ��
  USART_InitStructure.USART_BaudRate              =   bound;
  USART_InitStructure.USART_HardwareFlowControl   =   USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode                  =   USART_Mode_Tx|USART_Mode_Rx;
  USART_InitStructure.USART_Parity                =   USART_Parity_No;
  USART_InitStructure.USART_StopBits              =   USART_StopBits_1;
  USART_InitStructure.USART_WordLength            =   USART_WordLength_8b;
  USART_Init(UART5, &USART_InitStructure);
  USART_Cmd(UART5, ENABLE);
  //����5���ý���DMA
  USART_DMACmd(UART5, USART_DMAReq_Rx, ENABLE);    //����USART��DMA�ӿڣ�DMA1��������0��ͨ��4
  DMA_StructInit(&DMA_InitStructure);              //DMA������������ֵ

#if EN_UART5_DMA_SECOND_FIFO == 1
  DMA_InitStructure.DMA_Memory0BaseAddr   =   (uint32_t)&_UART5_DMA_RX_BUF[0][0];
  DMA_InitStructure.DMA_BufferSize        =   sizeof(_UART5_DMA_RX_BUF)/2;
#else
  DMA_InitStructure.DMA_Memory0BaseAddr   =   (uint32_t)&_UART5_DMA_RX_BUF;    //DMA���ջ���ַ
  DMA_InitStructure.DMA_BufferSize        =   sizeof(_UART5_DMA_RX_BUF);       //������������
#endif
  DMA_InitStructure.DMA_Channel           =   DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr=   (uint32_t)(&UART5->DR);       //�������ַ
  DMA_InitStructure.DMA_DIR               =   DMA_DIR_PeripheralToMemory;   //���赽�洢��
  DMA_InitStructure.DMA_PeripheralInc     =   DMA_PeripheralInc_Disable;    //�����ַ������
  DMA_InitStructure.DMA_MemoryInc         =   DMA_MemoryInc_Enable;         //�洢����ַ����
  DMA_InitStructure.DMA_MemoryDataSize    =   DMA_MemoryDataSize_Byte;      //�洢�����ݿ���
  DMA_InitStructure.DMA_PeripheralDataSize=   DMA_PeripheralDataSize_Byte;  //�������ݿ���
  DMA_InitStructure.DMA_Mode              =   DMA_Mode_Normal;              //�Ƿ�ѭ��ģʽ
  DMA_InitStructure.DMA_Priority          =   DMA_Priority_Medium  ;        //���ȼ��е�
  DMA_InitStructure.DMA_FIFOMode          =   DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold     =   DMA_FIFOThreshold_1QuarterFull;
  DMA_InitStructure.DMA_MemoryBurst       =   DMA_MemoryBurst_Single;       //�洢��ͻ�������δ���
  DMA_InitStructure.DMA_PeripheralBurst   =   DMA_PeripheralBurst_Single;   //����ͻ�������δ���
  DMA_Init(DMA1_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA1_Stream0, ENABLE);                        //ʹ��DMAͨ������


  //����Memory1,Memory0�ǵ�һ��ʹ�õ�Memory
#if EN_UART5_DMA_SECOND_FIFO == 1
  DMA_DoubleBufferModeConfig(DMA1_Stream0,  (uint32_t)&_UART5_DMA_RX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
  DMA_DoubleBufferModeCmd(DMA1_Stream0, ENABLE);
#endif
  NVIC_InitStructure.NVIC_IRQChannel						=	UART5_IRQn;          //����5�����ж�
  NVIC_InitStructure.NVIC_IRQChannelCmd					=	ENABLE;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	=	1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority			=	2;
  NVIC_Init(&NVIC_InitStructure);
  USART_ITConfig(UART5, USART_IT_IDLE, ENABLE);
  USART_Cmd(UART5, ENABLE);


  //����5���÷���DMA
  USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);   //����USART��DMA�ӿڣ�DMA1��������7��ͨ��4
  DMA_Cmd(DMA1_Stream7, DISABLE);                 // ��DMAͨ��
  DMA_DeInit(DMA1_Stream7);

  while(DMA_GetCmdStatus(DMA1_Stream7) != DISABLE) {}
  DMA_InitStructure.DMA_Channel = DMA_Channel_4;
  DMA_InitStructure.DMA_PeripheralBaseAddr	= (uint32_t)(&UART5->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr   	= (uint32_t)&UART5_DMA_TX_BUF[0];
  DMA_InitStructure.DMA_DIR 			    = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize			= sizeof(UART5_DMA_TX_BUF);   //�������ݷ��ڸ�������
  DMA_InitStructure.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode 				= DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority 			= DMA_Priority_Low ;
  DMA_InitStructure.DMA_FIFOMode 			= DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold 		= DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst 		= DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;
  DMA_Init(DMA1_Stream7,&DMA_InitStructure);
  DMA_Cmd(DMA1_Stream7, ENABLE);                        //ʹ��DMAͨ������

  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream7_IRQn;     // DMA�����ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   // ���ȼ�����
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
 
	DMA_Cmd(DMA_Streamx, DISABLE);                      //�ر�DMA���� 
	
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}	//ȷ��DMA���Ա�����  
		
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr);          //���ݴ�����  
 
	DMA_Cmd(DMA_Streamx, ENABLE);                      //����DMA���� 
}	  



void UART5_IRQHandler(void)
{
  static uint32_t this_time_rx_len5 = 0;
  if(USART_GetITStatus(UART5, USART_IT_IDLE) != RESET)
    {
      (void)UART5->SR;
      (void)UART5->DR;

#if EN_UART5_DMA_SECOND_FIFO == 1
      if(DMA_GetCurrentMemoryTarget(DMA1_Stream0) == 0)
        {
          DMA_Cmd(DMA1_Stream0, DISABLE);
          DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0);
          this_time_rx_len5 = BSP_UART5_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream0);

          DMA_SetCurrDataCounter(DMA1_Stream0, BSP_UART5_DMA_RX_BUF_LEN);
          DMA_MemoryTargetConfig (DMA1_Stream0,(uint32_t)&_UART5_DMA_RX_BUF[1][0],DMA_Memory_1);
          DMA_Cmd(DMA1_Stream0, ENABLE);

					UART5_Data_Receive_Process_0
        }
      //Target is Memory1
      else
        {
          DMA_Cmd(DMA1_Stream0, DISABLE);
          DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0);

          this_time_rx_len5 =BSP_UART5_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream0);

          DMA_SetCurrDataCounter(DMA1_Stream0, BSP_UART5_DMA_RX_BUF_LEN);
          DMA_MemoryTargetConfig (DMA1_Stream0,(uint32_t)&_UART5_DMA_RX_BUF[0][0],DMA_Memory_0);
          DMA_Cmd(DMA1_Stream0, ENABLE);
					
					UART5_Data_Receive_Process_1
        }
#else
      DMA_Cmd(DMA1_Stream0, DISABLE);                          //�رմ���5��DMA����ͨ��
      DMA_ClearFlag(DMA1_Stream0, DMA_FLAG_TCIF0 | DMA_FLAG_HTIF0);
      this_time_rx_len5 = BSP_UART5_DMA_RX_BUF_LEN - DMA_GetCurrDataCounter(DMA1_Stream0); //��ȡDMA_GetCurrDataCounterʣ��������

      DMA_SetCurrDataCounter(DMA1_Stream0, BSP_UART5_DMA_RX_BUF_LEN);      //���õ�ǰDMAʣ��������
      DMA_Cmd(DMA1_Stream0, ENABLE); 
				//��������5��DMA����ͨ��
				UART5_Data_Receive_Process
     
#endif

    }
	}

#endif
	








#if EN_USART6
void usart6_init()
{
    USART_InitTypeDef usart;
    GPIO_InitTypeDef  gpio;
    NVIC_InitTypeDef  nvic;
		DMA_InitTypeDef dma;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE); 
	
		//����1��Ӧ���Ÿ���ӳ��
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_USART6); //GPIOA9����ΪUSART1
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_USART6); //GPIOA10����ΪUSART1
		
		//USART1�˿�����
		gpio.GPIO_Pin 	= GPIO_Pin_6 | GPIO_Pin_7; //GPIOA9��GPIOA10
		gpio.GPIO_Mode 	= GPIO_Mode_AF;//���ù���
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
		gpio.GPIO_PuPd 	= GPIO_PuPd_NOPULL; //����
		GPIO_Init(GPIOC,&gpio); //��ʼ��PA9��PA10

    USART_DeInit(USART6);
//    USART_StructInit(&usart);
    usart.USART_BaudRate = 961200;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART6, &usart);   


		USART_DMACmd(USART6, USART_DMAReq_Rx, ENABLE);
    
		DMA_DeInit(DMA2_Stream1);
//    DMA_StructInit(&dma);
    dma.DMA_Channel = DMA_Channel_5;
    dma.DMA_PeripheralBaseAddr	= (uint32_t)(&USART6->DR);
    dma.DMA_Memory0BaseAddr   	= (uint32_t)&_USART6_DMA_RX_BUF[0];
    dma.DMA_DIR 			    = DMA_DIR_PeripheralToMemory;
    dma.DMA_BufferSize			= BSP_USART6_RX_BUF_LENGTH;//sizeof(USART1_DMA_RX_BUF);
    dma.DMA_PeripheralInc 		= DMA_PeripheralInc_Disable;
    dma.DMA_MemoryInc 			= DMA_MemoryInc_Enable;
    dma.DMA_PeripheralDataSize 	= DMA_PeripheralDataSize_Byte;
    dma.DMA_MemoryDataSize 		= DMA_MemoryDataSize_Byte;
    dma.DMA_Mode 				= DMA_Mode_Normal;
    dma.DMA_Priority 			= DMA_Priority_Medium;
    dma.DMA_FIFOMode 			= DMA_FIFOMode_Disable;
    dma.DMA_FIFOThreshold 		= DMA_FIFOThreshold_1QuarterFull;
    dma.DMA_MemoryBurst 		= DMA_MemoryBurst_Single;
    dma.DMA_PeripheralBurst 	= DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream1, &dma);
    DMA_Cmd(DMA2_Stream1, ENABLE);
		
    //����Memory1,Memory0�ǵ�һ��ʹ�õ�Memory
		//ʹ��˫������ģʽʱ�����Զ�ʹ��ѭ��ģʽ��DMA_SxCR �е� CIRC λ��״̬�ǡ���
		//�ء���������ÿ���������ʱ�����洢��ָ�롣
//    DMA_DoubleBufferModeConfig(DMA2_Stream6, (uint32_t)&USART6_DMA_TX_BUF[1][0], DMA_Memory_0);   //first used memory configuration
//    DMA_DoubleBufferModeCmd(DMA2_Stream6, ENABLE);
    
		nvic.NVIC_IRQChannel = USART6_IRQn;                          
		nvic.NVIC_IRQChannelPreemptionPriority = 3;   //pre-emption priority 
		nvic.NVIC_IRQChannelSubPriority = 3;		    //subpriority 
		nvic.NVIC_IRQChannelCmd = ENABLE;			
		NVIC_Init(&nvic);	
		USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);        //usart rx idle interrupt  enabled
//		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);        //usart rx idle interrupt  enabled
//		nvic.NVIC_IRQChannel = DMA2_Stream2_IRQn ;
//    nvic.NVIC_IRQChannelPreemptionPriority =3;
//    nvic.NVIC_IRQChannelSubPriority = 0;
//    nvic.NVIC_IRQChannelCmd = ENABLE; 
//    NVIC_Init(&nvic); 
//		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
		
		
    USART_Cmd(USART6,ENABLE);
}

//���ڽ����жϷ�����
void USART6_IRQHandler(void)
{
	static uint32_t this_time_rx_len6 = 0;
	if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
	{
		(void)USART6->SR;
		(void)USART6->DR;
		DMA_Cmd(DMA2_Stream1, DISABLE); 
		DMA_ClearFlag(DMA2_Stream1, DMA_FLAG_TCIF1 | DMA_FLAG_HTIF1);  //************************************
		this_time_rx_len6 = BSP_USART6_RX_BUF_LENGTH - DMA_GetCurrDataCounter(DMA2_Stream1);

		USART6_Data_Receive_Process
		
		DMA_SetCurrDataCounter(DMA2_Stream1,BSP_USART6_RX_BUF_LENGTH);
		DMA_Cmd(DMA2_Stream1, ENABLE);
	}       
}

#endif