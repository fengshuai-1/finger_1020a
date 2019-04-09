

#ifndef __SYSCOMMENT_H__
#define __SYSCOMMENT_H__

#ifdef _MAININC_
#define EXTERN

#else

#define EXTERN					extern
#endif


/* ͷ�ļ�  ------------------------------------------------------------*/
#include "stm32f10x.h"
#include"finger.h"

#define MATCH_FINGER_CNT		4
#define FINGER_ID_MAX			10	//�������ָ���û�����Ϊָ��ģ���ϵ�û�и�Ӧ�źţ�����Ҫ�����ϵ�¼����ָ��
#define DATA_READ_CNT			2		//��16 bit�洢�� ָ�Ƹ��� + ledģʽ


#define ADDR_MAX_OFFSET         6



typedef enum 
{
    LED_MODE_ON = 0,									//
    LED_MODE_OFF, 
    LED_MODE_DEF = 0XFF, 
} LEDMode_T;


    
typedef enum SAVE
{
    SAVE_INIT = 0, 
    SAVE_WRITE,
    SAVE_WAIT,
    SAVE_EXIT,
    SAVE_DEF = 0XFF, 
} Save_T;

typedef enum 
{
    TOUCH_MATCH_INIT = 0, 
    TOUCH_MATCH_AGAIN,									//
    TOUCH_ADD_USER, 									//
    TOUCH_DEL_USER, 									//
    TOUCH_WAIT, 
    TOUCH_CHECK,										//
    TOUCH_DEF = 0XFF, 
} TouchState_T;


typedef enum 
{
    TOUCH_SUB_INIT = 0, 
    TOUCH_SUB_ENTER,									//
    TOUCH_SUB_SETMODE_ONE,									//���ÿ��ظ�ģʽ
    TOUCH_SUB_AGAIN_PON,								//
    TOUCH_SUB_SETMODE_TWO,									//���ÿ��ظ�ģʽ
    TOUCH_SUB_AGAIN,									//
    TOUCH_SUB_WAIT, 
    TOUCH_SUB_EXIT, 
    TOUCH_SUB_DEF = 0XFF, 
} TouchSub_T;


typedef enum 
{
    LOCK_STATE_DETECT = 0,								//
    LOCK_STATE_DEBOUNSE,								//
    LOCK_STATE_SENT,									//
    LOCK_STATE_OFF, 									//
    LOCK_STATE_DEF = 0XFF, 
} LockState_T;


typedef struct 
{
    u16 	        u16FingerfCnt;						//
    u16 			u16LedMode;
} PackData_t;


typedef union   PACK_U
{
	PackData_t		Data;
	u8				u8FlashData[(DATA_READ_CNT) * 2];
	u16 			u16FlashData[DATA_READ_CNT];	//��ΪFlash��ȡΪ16��bitΪ��λ��������ת��ע���ֽ����д�С�˵�����
}FlashData_u;

typedef struct 
{
    volatile unsigned int u16Tick;						//
    unsigned int	u16LoadTime;						//

    unsigned long	u32TouchWaitTime;					//
    unsigned long	u32TouchOverTime;					//
    TouchState_T	TouchState;
    TouchSub_T		TouchSub;

    u16         	u16FingerfCnt;						//

    u16 			u16AddrOffset;
    u16             u16LedMode;							// LEDģʽ
    u16         	u16LedModeSave;						// LEDģʽ


    FlashData_u 	FlashData;

    
    u16             u16SaveTick;                        //��ʱ����״̬��Ϣʱ��
    u16             u16SaveWaitTick;                    //����״̬����ʱ
    u16 			bEnSaveData;						//ʹ�ܱ���
} SYS_TASK;


/* ȫ�ֱ��� -----------------------------------------------------------*/

/* ȫ�ֱ��� -----------------------------------------------------------*/
EXTERN vu8		mSysIWDGDog; //�����
EXTERN vu32 	mSysSoftDog; //�������� 
EXTERN vu16 	mSysTick; //������
EXTERN vu16 	mSysSec; //������
EXTERN vu16 	mTimeRFRX; //���ռ��-����

EXTERN SYS_TASK SysTask;



void MainTask(void);
void SysInit(void);

#endif

