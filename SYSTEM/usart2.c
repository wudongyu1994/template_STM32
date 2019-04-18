#include "delay.h"
#include "usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
 

//���ڽ��ջ����� 	
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 			//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�

//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������timer,����Ϊ����1����������.Ҳ���ǳ���timerû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA=0;

void USART2_IRQHandler(void)
{
	u8 res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//���յ�����
	{
		res =USART_ReceiveData(USART2);
		if((USART2_RX_STA&(1<<15))==0){              //�������һ������,��û�б�����,���ٽ�����������
			if(USART2_RX_STA<USART2_MAX_RECV_LEN){	//�����Խ�������
				TIM_SetCounter(TIM4,0);             //���������          				
				if(USART2_RX_STA==0) 				//������յ��ĵ�һ���ֽڣ���ʹ�ܶ�ʱ��4
					TIM_Cmd(TIM4,ENABLE);
				USART2_RX_BUF[USART2_RX_STA++]=res;	//��¼���յ���ֵ	 
			}else
				USART2_RX_STA|=1<<15;				//ǿ�Ʊ�ǽ������
		}
	}
}

USART_InitTypeDef USART_InitStructure;
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void usart2_init(u32 bound)
{  

	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); //����2ʱ��ʹ��

 	USART_DeInit(USART2);                           //��λ����2
   //USART2_TX   PA2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;      //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;           //��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = bound;                     //������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;             //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
 
	USART_Cmd(USART2, ENABLE);                  //ʹ�ܴ��� 
	
	//ʹ�ܽ����ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�   
	
	//�����ж����ȼ�
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	TIM4_Int_Init(99,7199);	//10ms�ж�
	USART2_RX_STA=0;		//����
	TIM_Cmd(TIM4,DISABLE);	//�رն�ʱ��4
}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART2_TX_BUF);		//�˴η������ݵĳ���
	for(j=0;j<i;j++)							//ѭ����������
	{
	  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	  USART_SendData(USART2,USART2_TX_BUF[j]); 
	} 
}

//����2�����ʺ�У��λ����
//bps:�����ʣ�1200~115200��
//parity:У��λ����0��ż2����1��
void usart2_set(u32 bound,u8 parity)
{
	USART_Cmd(USART2, DISABLE); //�رմ��� 
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	
	if(parity==0)//��У��
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
		USART_InitStructure.USART_Parity = USART_Parity_No;
	}else if(parity==2)//żУ��
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;    
		USART_InitStructure.USART_Parity = USART_Parity_Even;
	}else if(parity==1)//��У��
	{
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;    
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
	}
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
    USART_Cmd(USART2, ENABLE); //ʹ�ܴ��� 
}
 
//���ڽ���ʹ�ܿ���
//enable:0,�ر� 1,��
void usart2_rx(u8 enable)
{
	 USART_Cmd(USART2, DISABLE); //ʧ�ܴ��� 
	
	 if(enable)
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
	 }else
	 {
		 USART_InitStructure.USART_Mode = USART_Mode_Tx;//ֻ���� 
	 }
	 
	 USART_Init(USART2, &USART_InitStructure); //��ʼ������2
     USART_Cmd(USART2, ENABLE); //ʹ�ܴ��� 
	
}
