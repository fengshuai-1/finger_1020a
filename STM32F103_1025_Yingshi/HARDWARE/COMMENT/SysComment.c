
#include <string.h>

#include "usart1.h" 
#include "usart3.h" 
#include "delay.h"

#include "SysComment.h"
#include "stm32f10x_it.h"
#include "stm32f10x_iwdg.h"
#include "san_flash.h"
#include "finger.h"


//�ڲ�����
static vu16 	mDelay;
#define  POWER_ON_DELAY   250  //�ϵ�һ��ʱ����ȥʹ�� ָ��ģ�飬ʵ��1020A ��Ҫ200ms                     1020C ��Ҫ100ms ,ע�����Ĳ�����Ҫ���϶�β�������ֹоƬ������ɵ�Ӱ��


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
//�ڲ�����
void SysTickConfig(void);


/*******************************************************************************
* ����: SysTick_Handler
* ����: ϵͳʱ�ӽ���1MS
* �β�:		
* ����: ��
* ˵��: 
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
			IWDG_ReloadCounter();					/*ιSTM32����Ӳ����*/

			if ((++mSysSoftDog) > 10) /*��system  DOG 10S over*/
			{
				mSysSoftDog 		= 0;
				NVIC_SystemReset();
			}
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
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
* ����: Strcpy()
* ����: 
* �β�:		
* ����: ��
* ˵��: 
******************************************************************************/
void Strcpy(u8 * str1, u8 * str2, u8 len)
{
	for (; len > 0; len--)
	{
		*str1++ 			= *str2++;
	}
}


/*******************************************************************************
* ����: Strcmp()
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
* ����: Sys_DelayMS()
* ����: ϵͳ�ӳٺ���
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void Sys_DelayMS(uint16_t nms)
{
	mDelay				= nms + 1;

	while (mDelay != 0x0)
		;
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void Sys_IWDGConfig(u16 time)
{
	/* д��0x5555,�����������Ĵ���д�빦�� */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/* ����ʱ�ӷ�Ƶ,40K/64=0.625K()*/
	IWDG_SetPrescaler(IWDG_Prescaler_64);

	/* ι��ʱ�� TIME*1.6MS .ע�ⲻ�ܴ���0xfff*/
	IWDG_SetReload(time);

	/* ι��*/
	IWDG_ReloadCounter();

	/* ʹ�ܹ���*/
	IWDG_Enable();
}


/*******************************************************************************
* ����: Sys_IWDGReloadCounter
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void Sys_IWDGReloadCounter(void)
{
	mSysSoftDog 		= 0;						//ι��
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
* ����: 
* ����: У���
* �β�:		
* ����: ��
* ˵��: 
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
* ����: 
* ����: �ӻ�������
*		��ͷ + �ӻ���ַ + 	 �ֱ��ַ + ���� + ���ݰ�[3] + У��� + ��β
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void SlavePackageSend(u8 u8SlaveAddr, u8 u8GlassAddr, u8 u8Cmd, u8 * u8Par)
{
	u8 i				= 0;
	u8 u8SendArr[9] 	=
	{
		0
	};

	u8SendArr[0]		= 0xAA; 					//��ͷ
	u8SendArr[1]		= u8SlaveAddr;				//�ӻ���ַ
	u8SendArr[2]		= u8GlassAddr;				//�ֱ��ַ
	u8SendArr[3]		= u8Cmd;					//����
	u8SendArr[4]		= u8Par[0]; 				//����1
	u8SendArr[5]		= u8Par[1]; 				//����2
	u8SendArr[6]		= u8Par[2]; 				//����3

	u8SendArr[7]		= CheckSum(u8SendArr, 7);	//У���
	u8SendArr[8]		= 0x55; 					//��β


	for (i = 0; i < 8; i++)
	{
		USART_SendData(USART3, u8SendArr[i]);
	}
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
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
			SysTask.FlashData.Data.u16LedMode = SysTask.u16LedMode; //LEDģʽ
			u8Status = SAVE_WRITE;
			break;

		case SAVE_WRITE:
			if (FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, DATA_READ_CNT))
			{
				u8count++;
				u8Status			= SAVE_WAIT;
				SysTask.u16SaveWaitTick = 100;		// 100ms ��ʱ������д��
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

				if (u8count >= 7) //ÿҳ����8��
				{
					if (++SysTask.u16AddrOffset < ADDR_MAX_OFFSET) //��5ҳ
					{
						for (i = 0; i < 3; i++) //��д8�����
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
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void FingerTouchTask(void)
{
	//SearchResult seach;
	unsigned char u8Ensure;
	int retVal			= 0;
	static u8 u8MatchCnt = 0;						//ƥ��ʧ�ܴ�����Ĭ��ƥ��MATCH_FINGER_CNT�� 
	static u8 u8SetCnt	= 0;						//����ʧ�ܴ���

	switch (SysTask.TouchState)
	{
		case TOUCH_MATCH_INIT:
			if (PS_Sta) //��ָ�ư���
			{
				SysTask.u32TouchWaitTime = SysTask.u16Tick + POWER_ON_DELAY; //һ��ʱ���ټ�⣬ ��ָ��ģ��MCU�ϵ���ȫ ,ʵ��1020A��Ҫ200ms  1020c 100ms
				FINGER_POWER_ON();					//����ָ��ģ���Դ
				SysTask.TouchState	= TOUCH_MATCH_AGAIN;
				u8MatchCnt			= 0;
			}

			break;

		case TOUCH_MATCH_AGAIN:
			if (SysTask.u32TouchWaitTime == 0) //��ָ�ư���
			{
				u8Ensure			= Finger_Search(); //1��N����ָ�ƿ�

				if (u8Ensure == TRUE) //ƥ��ɹ�
				{
					//SysTask.u16FingerfCnt = seach.pageID;
					SysTask.u32TouchWaitTime = SysTask.u16Tick + 200; //��ʱһ��ʱ�� �ɹ�Ҳ����һֱ��
					SysTask.TouchSub	= TOUCH_SUB_INIT;
					SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��
					FINGER_POWER_OFF(); 			//�ر�ָ��ģ���Դ
				}
				else 
				{
					SysTask.u32TouchWaitTime = SysTask.u16Tick + 100; //��ʱһ��ʱ�� ����ֳ�ʶ���ˣ�������һ���˼�⣬

					//ƥ��ʧ��
					if (u8MatchCnt++ >= MATCH_FINGER_CNT) // 50 * 3 ��Լ200MS��
					{
						FINGER_POWER_OFF(); 		//�ر�ָ��ģ���Դ
						SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��
						SysTask.u32TouchWaitTime = SysTask.u16Tick + 500; //��ʱһ��ʱ�� ����ֳ�ʶ���ˣ�������һ���˼�⣬
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
							SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��
							SysTask.u32TouchWaitTime = SysTask.u16Tick + POWER_ON_DELAY; //��ʱһ��ʱ��
							return;
						}
						else if ((PS_Sta) && (SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_ON();		//����ָ��ģ���Դ
							SysTask.u32TouchWaitTime = POWER_ON_DELAY; //��ʱһ��ʱ����ȥ�����ظ�ģʽ
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
						if ((SysTask.u32TouchWaitTime == 0)) //ʱ�䵽ȥ�ɼ�
						{
							u8MatchCnt++;
							u8Ensure			= Finger_Enroll(SysTask.u16FingerfCnt);

							if (u8Ensure == TRUE)
							{
								u8MatchCnt			= 0;
								SysTask.u32TouchWaitTime = 300; //��ʱһ��ʱ����ȥ�ɼ�
								SysTask.TouchSub	= TOUCH_SUB_AGAIN_PON; //�����ڶ���		
								FINGER_POWER_OFF(); //�ر�ָ��ģ���Դ,�ȴ�¼��ڶ���ָ��		
							}
							else 
							{

								if (u8MatchCnt >= MATCH_FINGER_CNT) // 50 * 3 ��Լ200MS��
								{
									SysTask.u16FingerfCnt -= 1;
									FINGER_POWER_OFF(); //�ر�ָ��ģ���Դ

									//SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��	  
									SysTask.TouchSub	= TOUCH_SUB_INIT;
								}
								else 
								{
									//ƥ��ʧ������Ҫ����һ������ʾ����
									SysTask.u32TouchWaitTime = 100; //��ʱһ��ʱ���ٴ����
								}
							}
						}

						break;

					case TOUCH_SUB_AGAIN_PON:
						if ((PS_Sta) && (SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_ON();		//����ָ��ģ���Դ
							SysTask.u32TouchWaitTime = POWER_ON_DELAY; //��ʱһ��ʱ����ȥ�ɼ�
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
							u8Ensure			= Finger_Enroll(SysTask.u16FingerfCnt); //��������һ��ID�У�һ��ָ��һ��������������ע�⣬�����ָ�Ʋ�һ����ͬһ��

							if (u8Ensure == TRUE)
							{
								SysTask.u16SaveTick = 1; //1��󱣴�
								SysTask.TouchSub	= TOUCH_SUB_WAIT;
								SysTask.u32TouchWaitTime = 300; //��ʱһ��ʱ���˳�
							}
							else 
							{

								//ƥ��ʧ��
								if (u8MatchCnt++ >= MATCH_FINGER_CNT) // 50 * 3 ��Լ200MS��
								{
                                    Finger_Delete(SysTask.u16FingerfCnt - 1);  //���ʧ�ܣ�ɾ����һ��¼���
									SysTask.u16FingerfCnt -= 2;
									FINGER_POWER_OFF(); //�ر�ָ��ģ���Դ

									SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��	  
									SysTask.TouchSub	= TOUCH_SUB_INIT;
								}
								else 
								{
									//ƥ��ʧ������Ҫ����һ������ʾ����
									//SysTask.TouchSub	= TOUCH_SUB_ENTER;   //�ظ�ģʽ���ﲻ���ٴθ�
									SysTask.u32TouchWaitTime = 60; //��ʱһ��ʱ���ٴ����
								}
							}
						}

						break;

					case TOUCH_SUB_WAIT: // *
						if ((SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_OFF(); 	//�ر�ָ��ģ���Դ

							SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��	  
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
							FINGER_POWER_ON();		//����ָ��ģ���Դ
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

						u8Ensure = Finger_Clear(); //ɾ������ָ�ƿ�

						if (u8Ensure == TRUE)
						{
							SysTask.u16FingerfCnt = 0;
							SysTask.u16SaveTick = 1; //1��󱣴�
							SysTask.TouchSub	= TOUCH_SUB_WAIT;
							SysTask.u32TouchWaitTime = 100; //��ʱһ��ʱ���˳�
						}
						else //ɾ��ʧ�� ,
						{
							u8MatchCnt++;
							SysTask.u32TouchWaitTime = 100; //��ʱһ��ʱ���ٴ�ɾ��

							if (u8MatchCnt >= 3) //ʧ���˳�
							{
								SysTask.TouchSub	= TOUCH_SUB_WAIT;
							}
						}

						break;

					case TOUCH_SUB_WAIT: // *
						if ((SysTask.u32TouchWaitTime == 0))
						{
							FINGER_POWER_OFF(); 	//�ر�ָ��ģ���Դ
							SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��	  
							SysTask.TouchSub	= TOUCH_SUB_INIT;
						}

						break;

					default:
						break;
				}
			}
			break;

		case TOUCH_CHECK: //��ȡID����EEPROM�Ƿ�һ��, ����ָ�Ƶ�¼��ģʽΪ���ظ�
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
							SysTask.TouchSub	= TOUCH_SUB_WAIT; //ʧ���������ý�ȥ
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
								SysTask.u16SaveTick = 1; //1��󱣴�
							}
							else 
							{
								SysTask.u16FingerfCnt = 0;
								SysTask.u16SaveTick = 1; //1��󱣴�
							}
						}
					}

					SysTask.TouchSub = TOUCH_SUB_EXIT;
					break;

				case TOUCH_SUB_WAIT: // *
					if ((SysTask.u32TouchWaitTime == 0))
					{
						if (u8SetCnt >= 20) //����20��ʧ�ܣ��򷵻�
							SysTask.TouchSub = TOUCH_SUB_ENTER;
						else 
							SysTask.TouchSub = TOUCH_SUB_INIT;
					}

					break;

				case TOUCH_SUB_EXIT: // *
					if ((SysTask.u32TouchWaitTime == 0))
					{
						FINGER_POWER_OFF(); 		//�ر�ָ��ģ���Դ
						SysTask.TouchState	= TOUCH_MATCH_INIT; //����ƥ��   
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
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void KeyTask(void)
{
	if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12))
	{
		SysTask.TouchState	= TOUCH_ADD_USER;		//����ƥ��	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
	else if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13))
	{

		SysTask.TouchState	= TOUCH_DEL_USER;		//����ƥ��	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
	else if (!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14))
	{

		SysTask.TouchState	= TOUCH_MATCH_INIT;		//����ƥ��	 
		SysTask.TouchSub	= TOUCH_SUB_INIT;
        SysTask.u32TouchWaitTime = POWER_ON_DELAY;
	}
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void MainTask(void)
{
	SysSaveDataTask();
	LedTask();
	KeyTask();
	FingerTouchTask();								//ָ��ģ��
}


/*******************************************************************************
* ����: 
* ����: ���ָ�Ƹ���
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void Sys_EEPROMInit(void)
{
	int i;


	SysTask.u16AddrOffset = FLASH_ReadHalfWord(g_au32DataSaveAddr[0]); //��ȡ

	if ((SysTask.u16AddrOffset == 0xffff) || (SysTask.u16AddrOffset >= ADDR_MAX_OFFSET) ||
		 (SysTask.u16AddrOffset == 0)) //��ʼ״̬ flash��0xffff
	{
		SysTask.u16AddrOffset = 1;					//�ӵ�һҳ��ʼд��

		for (i = 0; i < 8; i++) //��д8�����
		{
			if (!FLASH_WriteMoreData(g_au32DataSaveAddr[0], &SysTask.u16AddrOffset, 1))
				break;

			delay_ms(20);
		}

		SysTask.FlashData.Data.u16LedMode = LED_MODE_ON; //LEDģʽ
		SysTask.FlashData.Data.u16FingerfCnt = 0;	//
		FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	}


	FLASH_ReadMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);

	SysTask.u16FingerfCnt = SysTask.FlashData.Data.u16FingerfCnt;
	SysTask.u16LedMode	= SysTask.FlashData.Data.u16LedMode;


	if (SysTask.u16FingerfCnt > FINGER_ID_MAX)
	{
#if ENABLE_DEBUG

		//		mySerial.println("EEPROM �������ݳ�������");
		//		mySerial.print("SysTask.u16FingerfCnt :");
		//		mySerial.println(SysTask.u16FingerfCnt);
#endif

		SysTask.u16FingerfCnt = 0;
		SysTask.FlashData.Data.u16LedMode = LED_MODE_ON; //LEDģʽ
		SysTask.FlashData.Data.u16FingerfCnt = SysTask.u16FingerfCnt; //
		FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	}

#if ENABLE_DEBUG

	//	mySerial.print("SysTask.u16FingerfCnt :");
	//	mySerial.println(SysTask.u16FingerfCnt);
#endif
}


/*******************************************************************************
* ����: 
* ����: 
* �β�:		
* ����: ��
* ˵��: 
*******************************************************************************/
void SysInit(void)
{
	FINGER_POWER_ON();								//��ָ��ģ��DSP�ϵ磬���������һ��ָ��ID �Ƿ���  EEPRO	  M  һ��
	Sys_EEPROMInit();

	SysTask.u16LedModeSave = 0xff;					//Ϊ��һ��ʼ����������
	SysTask.TouchState	= TOUCH_CHECK;
	SysTask.u32TouchWaitTime = POWER_ON_DELAY; 				//
	SysTask.u32TouchOverTime = 0;					//

	//	FLASH_WriteMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
	//	FLASH_ReadMoreData(g_au32DataSaveAddr[SysTask.u16AddrOffset], SysTask.FlashData.u16FlashData, sizeof(SysTask.FlashData.u16FlashData) / 2);
}


