

#ifndef __SYSCOMMENT_H__
#define __SYSCOMMENT_H__

#ifdef _MAININC_
#define EXTERN

#else

#define EXTERN					extern
#endif


/* 头文件  ------------------------------------------------------------*/
#include "stm32f10x.h"
#include"finger.h"

#define MATCH_FINGER_CNT		4
#define FINGER_ID_MAX			10	//最多三个指纹用户，因为指纹模块上电没有感应信号，所以要重新上电录两次指纹
#define DATA_READ_CNT			2		//按16 bit存储， 指纹个数 + led模式


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
    TOUCH_SUB_SETMODE_ONE,									//设置可重复模式
    TOUCH_SUB_AGAIN_PON,								//
    TOUCH_SUB_SETMODE_TWO,									//设置可重复模式
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
	u16 			u16FlashData[DATA_READ_CNT];	//因为Flash读取为16个bit为单位，后续再转换注意字节序还有大小端的问题
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
    u16             u16LedMode;							// LED模式
    u16         	u16LedModeSave;						// LED模式


    FlashData_u 	FlashData;

    
    u16             u16SaveTick;                        //延时保存状态信息时间
    u16             u16SaveWaitTick;                    //保存状态机延时
    u16 			bEnSaveData;						//使能保存
} SYS_TASK;


/* 全局变量 -----------------------------------------------------------*/

/* 全局变量 -----------------------------------------------------------*/
EXTERN vu8		mSysIWDGDog; //软狗标记
EXTERN vu32 	mSysSoftDog; //软狗计数器 
EXTERN vu16 	mSysTick; //节拍器
EXTERN vu16 	mSysSec; //节拍器
EXTERN vu16 	mTimeRFRX; //接收间隔-仿真

EXTERN SYS_TASK SysTask;



void MainTask(void);
void SysInit(void);

#endif

