
#include <string.h>

#include "usart1.h" 
#include "usart3.h" 
#include "delay.h"

#include "SysComment.h"
#include "stm32f10x_it.h"
#include "stm32f10x_iwdg.h"
#include "san_flash.h"
#include "finger.h"


//内部变量
static vu16 	mDelay;
#define  POWER_ON_DELAY   250  //上电一段时间再去使用 指纹模块，实际1020A 需要200ms                     1020C 需要100ms ,注意后面的操作还要加上多次操作，防止芯片差异造成的影响


// STM32C8T  64k 0X10000
u32 			g_au32DataSaveAddr[6] =
{
	0X0800E400, 
	0X0800E800, 
	0X0800EC00, 
	0x0800F000, 
	0x0800F400, 
	0x0800F800
};


// STM32ZE	 512K  0X80000
//u32			g_au32DataSaveAddr[6] =   
//{
//	0x0807D000, 
//	0x0807D800, 
//	0x0807E000, 
//	0x0807E800, 
//	0x0807F000, 
//	0x0807F800
//};
//内部函数
void SysTickConfig(void);


/*******************************************************************************
* 名称: SysTick_Handler
* 功能: 系统时钟节拍1MS
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SysTick_Handler(void)
{
	static u16		Tick_1S = 0;

	mSysTick++;
	mSysSec++;
	mTimeRFRX++;

	if (mDelay)
		mDelay--;

	if (++Tick_1S >= 1000)
	{
		Tick_1S 			= 0;

		if (mSysIWDGDog)
		{
			IWDG_ReloadCounter();					/*喂STM32内置硬件狗*/

			if ((++mSysSoftDog) > 10) /*软狗system  DOG 10S over*/
			{
				mSysSoftDog 		= 0;
				NVIC_SystemReset();
			}
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
void DelayUs(uint16_t nCount)
{
	u32 			del = nCount * 5;

	//48M 0.32uS
	//24M 0.68uS
	//16M 1.02us
	while (del--)
		;
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void DelayMs(uint16_t nCount)
{
	unsigned int	ti;

	for (; nCount > 0; nCount--)
	{
		for (ti = 0; ti < 4260; ti++)
			; //16M/980-24M/1420 -48M/2840
	}
}


/*******************************************************************************
* 名称: Strcpy()
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
******************************************************************************/
void Strcpy(u8 * str1, u8 * str2, u8 len)
{
	for (; len > 0; len--)
	{
		*str1++ 			= *str2++;
	}
}


/*******************************************************************************
* 名称: Strcmp()
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
******************************************************************************/
int Strcmp(u8 * str1, u8 * str2, u8 len)
{
	for (; len > 0; len--)
	{
		if (*str1++ != *str2++)
			return FALSE;
	}

	return TRUE;
}


/*******************************************************************************
* 名称: Sys_DelayMS()
* 功能: 系统延迟函数
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void Sys_DelayMS(uint16_t nms)
{
	mDelay				= nms + 1;

	while (mDelay != 0x0)
		;
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void Sys_IWDGConfig(u16 time)
{
	/* 写入0x5555,用于允许狗狗寄存器写入功能 */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* 狗狗时钟分频,40K/64=0.625K()*/
	IWDG_SetPrescaler(IWDG_Prescaler_64);

	/* 喂狗时间 TIME*1.6MS .注意不能大于0xfff*/
	IWDG_SetReload(time);

	/* 喂狗*/
	IWDG_ReloadCounter();

	/* 使能狗狗*/
	IWDG_Enable();
}


/*******************************************************************************
* 名称: Sys_IWDGReloadCounter
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void Sys_IWDGReloadCounter(void)
{
	mSysSoftDog 		= 0;						//喂软狗
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SysTickConfig(void)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

	/* Setup SysTick Timer for 1ms interrupts  */
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture error */
		while (1)
			;
	}

	/* Configure the SysTick handler priority */
	NVIC_SetPriority(SysTick_IRQn, 0x0);

#if (								SYSINFOR_PRINTF == 1)
	printf("SysTickConfig:Tick=%d/Second\r\n", 1000);
#endif
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void LedInit(void) //
{
	//	GPIO_InitTypeDef GPIO_InitStructure;
	//	RCC_APB2PeriphClockCmd(LED_RCC_CLOCKGPIO, ENABLE);
	//	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	//	GPIO_SetBits(LED_GPIO, LED_PIN);
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void LedTask(void)
{
	//	if (SysTask.LED_Mode != SysTask.LED_ModeSave)
	//	{
	//		if (SysTask.LED_Mode == LED_MODE_ON)
	//			GPIO_ResetBits(LED_GPIO, LED_PIN);
	//		else 
	//			GPIO_SetBits(LED_GPIO, LED_PIN);
	//		SysTask.LED_ModeSave = SysTask.LED_Mode;
	//	}
}


/*******************************************************************************
* 名称: 
* 功能: 校验和
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
u8 CheckSum(u8 * pu8Addr, u8 u8Count)
{
	u8 i				= 0;
	u8 u8Result 		= 0;

	for (i = 0; i < u8Count; i++)
	{
		u8Result			+= pu8Addr[i];
	}

	return ~ u8Result;
}


/*******************************************************************************
* 名称: 
* 功能: 从机包发送
*		包头 + 从机地址 + 	 手表地址 + 命令 + 数据包[3] + 校验和 + 包尾
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SlavePackageSend(u8 u8SlaveAddr, u8 u8GlassAddr, u8 u8Cmd, u8 * u8Par)
{
	u8 i				= 0;
	u8 u8SendArr[9] 	=
	{
		0
	};

	u8SendArr[0]		= 0xAA; 					//包头
	u8SendArr[1]		= u8SlaveAddr;				//从机地址
	u8SendArr[2]		= u8GlassAddr;				//手表地址
	u8SendArr[3]		= u8Cmd;					//命令
	u8SendArr[4]		= u8Par[0]; 				//参数1
	u8SendArr[5]		= u8Par[1]; 				//参数2
	u8SendArr[6]		= u8Par[2]; 				//参数3

	u8SendArr[7]		= CheckSum(u8SendArr, 7);	//校验和
	u8SendArr[8]		= 0x55; 					//包尾


	for (i = 0; i < 8; i++)
	{
		USART_SendData(USART3, u8SendArr[i]);
	}
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SysSaveDataTask(void)
{
	static u8 u8Status	= 0;
	static u8 u8count	= 0;
	u8 i;

	if (SysTask.bEnSaveData == FALSE)
		return;

	switch (u8Status)
	{
		case SAVE_INIT:
			SysTask.FlashData.Data.u16FingerfCnt = SysTask.u16FingerfCnt; //
			SysTask.FlashData.Data.u16LedMode = SysTask.u16LedMode; //LED模式
			u8Status = SAVE_WRITE;
			break;

		case SAVE_WRITE:
			if (FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, DATA_READ_CNT))
			{
				u8count++;
				u8Status			= SAVE_WAIT;
				SysTask.u16SaveWaitTick = 100;		// 100ms 延时再重新写入
			}
			else 
			{
				u8Status			= SAVE_EXIT;
			}

			break;

		case SAVE_WAIT:
			if (SysTask.u16SaveWaitTick == 0)
			{
				u8Status			= SAVE_WRITE;

				if (u8count >= 7) //每页尝试8次
				{
					if (++SysTask.u16AddrOffset < ADDR_MAX_OFFSET) //共5页
					{
						for (i = 0; i < 3; i++) //重写8次最多
						{
							if (!FLASH_WriteMoreData(g_au32DataSaveAddr[0], &SysTask.u16AddrOffset, 1))
							{
								u8count = 0;
                                break;
                             }
							delay_ms(20);
						}
					}
					else 
					{
						u8Status			= SAVE_EXIT;
					}
				}
			}

			break;

		case SAVE_EXIT:
			SysTask.bEnSaveData = FALSE;
			u8Status = SAVE_INIT;
			u8count = 0;
			break;

		default:
			u8Status = SAVE_EXIT;
			break;
	}
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void FingerTouchTask(void)
{
	//SearchResult seach;
	unsigned char u8Ensure;
	int retVal			= 0;
	static u8 u8MatchCnt = 0;						//匹配失败次数，默认匹配MATCH_FINGER_CNT次 
	static u8 u8SetCnt	= 0;						//设置失败次数

	switch (SysTask.TouchState)
	{
		case TOUCH_MATCH_INIT:
			if (PS_Sta) //有指纹按下
			{
				SysTask.u32TouchWaitTime = SysTask.u16Tick + POWER_ON_DELAY; //一段时间再检测， 让指纹模块MCU上电完全 ,实测1020A需要200ms  1020c 100ms
				FINGER_POWER_ON();					//开启指纹模块电源
				SysTask.TouchState	= TOUCH_MATCH_AGAIN;
				u8MatchCnt			= 0;
			}

			break;

		case TOUCH_MATCH_AGAIN:
			if (SysTask.u32TouchWaitTime == 0) //有指纹按下
			{
				u8Ensure			= Finger_Search(); //1：N搜索指纹库

				if (u8Ensure == TRUE) //匹配成功
				{
					//SysTask.u16FingerfCnt = seach.pageID;
					SysTask.u32TouchWaitTime = SysTask.u16Tick + 200; //延时一定时间 成功也不能一直发
					SysTask.TouchSub	= TOUCH_SUB_INIT;
					SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配
					FINGER_POWER_OFF(); 			//关闭指纹模块电源
				}
				else 
				{
					SysTask.u32TouchWaitTime = SysTask.u16Tick + 100; //延时一定时间 如果手臭识别不了，再想来一个人检测，

					//匹配失败
					if (u8MatchCnt++ >= MATCH_FINGER_CNT) // 50 * 3 大约200MS秒
					{
						FINGER_POWER_OFF(); 		//关闭指纹模块电源
						SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配
						SysTask.u32TouchWaitTime = SysTask.u16Tick + 500; //延时一定时间 如果手臭识别不了，再想来一个人检测，
					}
				}
			}

			break;

		case TOUCH_ADD_USER:
			{
				switch (SysTask.TouchSub)
				{
					case TOUCH_SUB_INIT: // *
						if (SysTask.u16FingerfCnt > FINGER_ID_MAX - 2)
						{
							SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配
							SysTask.u32TouchWaitTime = SysTask.u16Tick + POWER_ON_DELAY; //延时一定时间
							return;
						}
						else if ((PS_Sta) && (SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_ON();		//开启指纹模块电源
							SysTask.u32TouchWaitTime = POWER_ON_DELAY; //延时一定时间再去设置重复模式
							u8MatchCnt			= 0;
							SysTask.u16FingerfCnt++;
							SysTask.TouchSub	= TOUCH_SUB_SETMODE_ONE;
                            u8SetCnt = 0;
						}

						break;
					case TOUCH_SUB_SETMODE_ONE: // *
                        if (SysTask.u32TouchWaitTime == 0)
                        {
                            u8SetCnt++;
                        
                            if (!Finger_ModeSet())
                            {
                                SysTask.TouchSub    = TOUCH_SUB_ENTER;
                            }
                            else 
                            {
                                if(u8SetCnt <= 8)
                                    SysTask.u32TouchWaitTime = 50;
                                else
                                    SysTask.TouchSub    = TOUCH_SUB_ENTER;
                            }
                        }
						break;
                        

					case TOUCH_SUB_ENTER: //
						if ((SysTask.u32TouchWaitTime == 0)) //时间到去采集
						{
							u8MatchCnt++;
							u8Ensure			= Finger_Enroll(SysTask.u16FingerfCnt);

							if (u8Ensure == TRUE)
							{
								u8MatchCnt			= 0;
								SysTask.u32TouchWaitTime = 300; //延时一定时间再去采集
								SysTask.TouchSub	= TOUCH_SUB_AGAIN_PON; //跳到第二步		
								FINGER_POWER_OFF(); //关闭指纹模块电源,等待录入第二次指纹		
							}
							else 
							{

								if (u8MatchCnt >= MATCH_FINGER_CNT) // 50 * 3 大约200MS秒
								{
									SysTask.u16FingerfCnt -= 1;
									FINGER_POWER_OFF(); //关闭指纹模块电源

									//SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配	  
									SysTask.TouchSub	= TOUCH_SUB_INIT;
								}
								else 
								{
									//匹配失败这里要进行一定的提示操作
									SysTask.u32TouchWaitTime = 100; //延时一定时间再次添加
								}
							}
						}

						break;

					case TOUCH_SUB_AGAIN_PON:
						if ((PS_Sta) && (SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_ON();		//开启指纹模块电源
							SysTask.u32TouchWaitTime = POWER_ON_DELAY; //延时一定时间再去采集
							u8MatchCnt			= 0;
							SysTask.u16FingerfCnt++;
							SysTask.TouchSub	= TOUCH_SUB_SETMODE_TWO;
                            u8SetCnt = 0;
						}

						break;
					case TOUCH_SUB_SETMODE_TWO: // *
                        if (SysTask.u32TouchWaitTime == 0)
                        {
                            u8SetCnt++;
                        
                            if (!Finger_ModeSet())
                            {
                                SysTask.TouchSub    = TOUCH_SUB_AGAIN;
                            }
                            else 
                            {
                                if(u8SetCnt <= 8)
                                    SysTask.u32TouchWaitTime = 50;
                                else
                                    SysTask.TouchSub    = TOUCH_SUB_AGAIN;
                            }
                        }
						break;

					case TOUCH_SUB_AGAIN:
						if ((SysTask.u32TouchWaitTime == 0)) //
						{
							u8Ensure			= Finger_Enroll(SysTask.u16FingerfCnt); //存入另外一个ID中，一个指纹一共存两个，，，注意，这里的指纹不一定是同一个

							if (u8Ensure == TRUE)
							{
								SysTask.u16SaveTick = 1; //1秒后保存
								SysTask.TouchSub	= TOUCH_SUB_WAIT;
								SysTask.u32TouchWaitTime = 300; //延时一定时间退出
							}
							else 
							{

								//匹配失败
								if (u8MatchCnt++ >= MATCH_FINGER_CNT) // 50 * 3 大约200MS秒
								{
                                    Finger_Delete(SysTask.u16FingerfCnt - 1);  //添加失败，删除第一个录入的
									SysTask.u16FingerfCnt -= 2;
									FINGER_POWER_OFF(); //关闭指纹模块电源

									SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配	  
									SysTask.TouchSub	= TOUCH_SUB_INIT;
								}
								else 
								{
									//匹配失败这里要进行一定的提示操作
									//SysTask.TouchSub	= TOUCH_SUB_ENTER;   //重复模式这里不用再次搞
									SysTask.u32TouchWaitTime = 60; //延时一定时间再次添加
								}
							}
						}

						break;

					case TOUCH_SUB_WAIT: // *
						if ((SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_OFF(); 	//关闭指纹模块电源

							SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配	  
							SysTask.TouchSub	= TOUCH_SUB_INIT;
						}

						break;

					default:
						break;
				}
			}
			break;

		case TOUCH_DEL_USER:
			{
				switch (SysTask.TouchSub)
				{
					case TOUCH_SUB_INIT: // *
						if (SysTask.u32TouchWaitTime == 0)
						{
							FINGER_POWER_ON();		//开启指纹模块电源
							u8MatchCnt			= 0;
							SysTask.TouchSub	= TOUCH_SUB_ENTER;
							SysTask.u32TouchWaitTime = POWER_ON_DELAY; //
						}

						break;

					case TOUCH_SUB_ENTER: //
						if (SysTask.u32TouchWaitTime)
						{
							break;
						}

						u8Ensure = Finger_Clear(); //删除整个指纹库

						if (u8Ensure == TRUE)
						{
							SysTask.u16FingerfCnt = 0;
							SysTask.u16SaveTick = 1; //1秒后保存
							SysTask.TouchSub	= TOUCH_SUB_WAIT;
							SysTask.u32TouchWaitTime = 100; //延时一定时间退出
						}
						else //删除失败 ,
						{
							u8MatchCnt++;
							SysTask.u32TouchWaitTime = 100; //延时一定时间再次删除

							if (u8MatchCnt >= 3) //失败退出
							{
								SysTask.TouchSub	= TOUCH_SUB_WAIT;
							}
						}

						break;

					case TOUCH_SUB_WAIT: // *
						if ((SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_OFF(); 	//关闭指纹模块电源
							SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配	  
							SysTask.TouchSub	= TOUCH_SUB_INIT;
						}

						break;

					default:
						break;
				}
			}
			break;

		case TOUCH_CHECK: //读取ID看与EEPROM是否一致, 设置指纹的录入模式为可重复
			if (SysTask.u32TouchWaitTime)
				return;

			switch (SysTask.TouchSub)
			{
				case TOUCH_SUB_INIT: // *
					if (SysTask.u32TouchWaitTime == 0)
					{
						u8SetCnt++;

						if (!Finger_ModeSet())
						{
							SysTask.TouchSub	= TOUCH_SUB_ENTER;
						}
						else 
						{
							SysTask.u32TouchWaitTime = 50;
							SysTask.TouchSub	= TOUCH_SUB_WAIT; //失败重新设置进去
						}
					}

					break;

				case TOUCH_SUB_ENTER: //
					if (SysTask.u32TouchWaitTime)
					{
						break;
					}

					retVal = Finger_Read();

					if (retVal != -1)
					{
						if ((unsigned int)
						retVal != SysTask.u16FingerfCnt)
						{
							if (retVal <= FINGER_ID_MAX)
							{
								SysTask.u16FingerfCnt = retVal;
								SysTask.u16SaveTick = 1; //1秒后保存
							}
							else 
							{
								SysTask.u16FingerfCnt = 0;
								SysTask.u16SaveTick = 1; //1秒后保存
							}
						}
					}

					SysTask.TouchSub = TOUCH_SUB_EXIT;
					break;

				case TOUCH_SUB_WAIT: // *
					if ((SysTask.u32TouchWaitTime == 0))
					{
						if (u8SetCnt >= 20) //设置20次失败，则返回
							SysTask.TouchSub = TOUCH_SUB_ENTER;
						else 
							SysTask.TouchSub = TOUCH_SUB_INIT;
					}

					break;

				case TOUCH_SUB_EXIT: // *
					if ((SysTask.u32TouchWaitTime == 0))
					{
						FINGER_POWER_OFF(); 		//关闭指纹模块电源
						SysTask.TouchState	= TOUCH_MATCH_INIT; //重新匹配   
						SysTask.TouchSub	= TOUCH_SUB_INIT;
                        u8SetCnt = 0;
					}

					break;

				default:
					break;
			}

			break;

		default:
			break;
	}
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void KeyTask(void)
{
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
	{
		SysTask.TouchState	= TOUCH_ADD_USER;		//重新匹配	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
	else if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
	{

		SysTask.TouchState	= TOUCH_DEL_USER;		//重新匹配	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
	else if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
	{

		SysTask.TouchState	= TOUCH_MATCH_INIT;		//重新匹配	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void MainTask(void)
{
	SysSaveDataTask();
	LedTask();
	KeyTask();
	FingerTouchTask();								//指纹模块
}


/*******************************************************************************
* 名称: 
* 功能: 检查指纹个数
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void Sys_EEPROMInit(void)
{
	int i;


	SysTask.u16AddrOffset = FLASH_ReadHalfWord(g_au32DataSaveAddr[0]); //读取

	if ((SysTask.u16AddrOffset == 0xffff) || (SysTask.u16AddrOffset >= ADDR_MAX_OFFSET) ||
		 (SysTask.u16AddrOffset == 0)) //初始状态 flash是0xffff
	{
		SysTask.u16AddrOffset = 1;					//从第一页开始写入

		for (i = 0; i < 8; i++) //重写8次最多
		{
			if (!FLASH_WriteMoreData(g_au32DataSaveAddr[0], &SysTask.u16AddrOffset, 1))
				break;

			delay_ms(20);
		}

		SysTask.FlashData.Data.u16LedMode = LED_MODE_ON; //LED模式
		SysTask.FlashData.Data.u16FingerfCnt = 0;	//
		FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	}


	FLASH_ReadMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);

	SysTask.u16FingerfCnt = SysTask.FlashData.Data.u16FingerfCnt;
	SysTask.u16LedMode	= SysTask.FlashData.Data.u16LedMode;


	if (SysTask.u16FingerfCnt > FINGER_ID_MAX)
	{
#if ENABLE_DEBUG

		//		mySerial.println("EEPROM 保存数据出错！！！");
		//		mySerial.print("SysTask.u16FingerfCnt :");
		//		mySerial.println(SysTask.u16FingerfCnt);
#endif

		SysTask.u16FingerfCnt = 0;
		SysTask.FlashData.Data.u16LedMode = LED_MODE_ON; //LED模式
		SysTask.FlashData.Data.u16FingerfCnt = SysTask.u16FingerfCnt; //
		FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	}

#if ENABLE_DEBUG

	//	mySerial.print("SysTask.u16FingerfCnt :");
	//	mySerial.println(SysTask.u16FingerfCnt);
#endif
}


/*******************************************************************************
* 名称: 
* 功能: 
* 形参:		
* 返回: 无
* 说明: 
*******************************************************************************/
void SysInit(void)
{
	FINGER_POWER_ON();								//给指纹模块DSP上电，，后面会检查一次指纹ID 是否与  EEPRO	  M  一致
	Sys_EEPROMInit();

	SysTask.u16LedModeSave = 0xff;					//为了一开始可以跑起来
	SysTask.TouchState	= TOUCH_CHECK;
	SysTask.u32TouchWaitTime = POWER_ON_DELAY; 				//
	SysTask.u32TouchOverTime = 0;					//

	//	FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	//	FLASH_ReadMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
}


