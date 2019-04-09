	 
#include <string.h>

#include "delay.h"
#include "sys.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"

#include "timer.h"
//#include "pwm.h"


#define _MAININC_
#include "SysComment.h"
#undef _MAININC_
#define usart2_baund			57600//����2�����ʣ�����ָ��ģ�鲨���ʸ��ģ�ע�⣺ָ��ģ��Ĭ��57600��



void SysTickTask(void);


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void KeyGPIOInit(void)
{
	/* ����IOӲ����ʼ���ṹ����� */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��(����)���Ŷ�ӦIO�˿�ʱ��                         + */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


int main(void)
{
    
	delay_init();									//��ʱ������ʼ��	  
	NVIC_Configuration();							//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ� ��������ֻ����һ��;				 

//    usart1_init(115200);
//    Usart1_SendByte(0x88);
 
    usart3_init(115200); //  ��������LED
    Usart3_SendByte(0x66);
	
//    Usart2_SendByte(0x66);
	FingerGPIO_Init();								//��ʼ��FR��״̬����  ʶ��ָ�ư�ѹ����ߵ�ƽ����Ϊ���ù�ϵ��ʼ�������ŵ�KEY_GPIO_Init����
	KeyGPIOInit();
    
    SysInit();
	usart2_init(19200);						//��ʼ������2,������ָ��ģ��ͨѶ
	GENERAL_TIMx_Configuration();
	while (1)
	{
		MainTask();

		if (SysTask.u16Tick)
		{
			SysTask.u16Tick--;
			SysTickTask();

		}
	}
    
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void SysTickTask(void)
{
	vu16 static 	u16SecTick = 0; 				//�����

	if (u16SecTick++ >= 1000) //������
	{
		u16SecTick			= 0;
        
       if (SysTask.u16SaveTick)
        {   
    		SysTask.u16SaveTick--;
            if(SysTask.u16SaveTick == 0)
                SysTask.bEnSaveData = TRUE;  //��������
        }
	}

    if(SysTask.u16SaveWaitTick)
        SysTask.u16SaveWaitTick--;

    
    if(SysTask.u32TouchWaitTime)
        SysTask.u32TouchWaitTime--;

	//			if (state)
	//			{
	//				state				= 0;
	//				GPIO_ResetBits(GPIOC, GPIO_Pin_13);
	//			}
	//			else 
	//			{
	//				state				= 1;
	//				GPIO_SetBits(GPIOC, GPIO_Pin_13);
	//			}
}


