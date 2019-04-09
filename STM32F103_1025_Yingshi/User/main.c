	 
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
#define usart2_baund			57600//串口2波特率，根据指纹模块波特率更改（注意：指纹模块默认57600）



void SysTickTask(void);


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void KeyGPIOInit(void)
{
	/* 定义IO硬件初始化结构体变量 */
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能(开启)引脚对应IO端口时钟                         + */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//

	GPIO_Init(GPIOB, &GPIO_InitStructure);
}


int main(void)
{
    
	delay_init();									//延时函数初始化	  
	NVIC_Configuration();							//设置NVIC中断分组2:2位抢占优先级，2位响应优先级 整个程序只设置一次;				 

//    usart1_init(115200);
//    Usart1_SendByte(0x88);
 
    usart3_init(115200); //  用作控制LED
    Usart3_SendByte(0x66);
	
//    Usart2_SendByte(0x66);
	FingerGPIO_Init();								//初始化FR读状态引脚  识别到指纹按压输出高电平，因为复用关系初始化动作放到KEY_GPIO_Init里面
	KeyGPIOInit();
    
    SysInit();
	usart2_init(19200);						//初始化串口2,用于与指纹模块通讯
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
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SysTickTask(void)
{
	vu16 static 	u16SecTick = 0; 				//秒计数

	if (u16SecTick++ >= 1000) //秒任务
	{
		u16SecTick			= 0;
        
       if (SysTask.u16SaveTick)
        {   
    		SysTask.u16SaveTick--;
            if(SysTask.u16SaveTick == 0)
                SysTask.bEnSaveData = TRUE;  //开启保存
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


