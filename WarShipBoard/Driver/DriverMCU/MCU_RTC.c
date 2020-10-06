/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:20:25
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 13:43:55
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_RTC.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "MCU_RTC.h"

//RTC初始化
D_ERROR_CODE MCU_RTC_Init(void)
{
    RTC_InitTypeDef RTC_InitStructure;
	uint16_t retry=0X1FFF; 
    //使能PWR时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    //使能后备寄存器访问 
	PWR_BackupAccessCmd(ENABLE);	
	//是否第一次配置?
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x55AA)		
	{
        //LSE 开启 
		RCC_LSEConfig(RCC_LSE_ON);   
        //检查指定的RCC标志位设置与否,等待低速晶振就绪
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	
        {
            retry++;
            CoreDelayMs(10);
        }
        //LSE 开启失败. 
		if(retry==0)
        {
            return D_ERROR_CODE_MCU_RTC_LSE_START;	
        }
        //设置RTC时钟(RTCCLK),选择LSE作为RTC时钟   	
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	
        //使能RTC时钟 	 
		RCC_RTCCLKCmd(ENABLE);	
        //RTC异步分频系数(1~0X7F)
        RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
        //RTC同步分频系数(0~7FFF)
        RTC_InitStructure.RTC_SynchPrediv  = 0xFF;
        //RTC设置为,24小时格式
        RTC_InitStructure.RTC_HourFormat   = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStructure);
        //设置时间
		MCU_RTC_SetTime(23,59,56);	
        //设置日期
		MCU_RTC_SetDate(20,10,6,2);		
        //标记已经初始化过了
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x55AA);	
	}
	return D_ERROR_CODE_NONE;
}	
					
//RTC时间设置
D_ERROR_CODE MCU_RTC_SetTime(uint8_t hour,uint8_t min,uint8_t sec)
{
    RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	//设置时间
	RTC_TimeTypeInitStructure.RTC_Hours = hour;
	RTC_TimeTypeInitStructure.RTC_Minutes = min;
	RTC_TimeTypeInitStructure.RTC_Seconds = sec;
	RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
	//设置时间
    if(SUCCESS != RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure))
    {
        return D_ERROR_CODE_MCU_RTC_SET_TIME;
    } 
    return D_ERROR_CODE_NONE;
}

//RTC日期设置		
D_ERROR_CODE MCU_RTC_SetDate(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
    RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date = date;
	RTC_DateTypeInitStructure.RTC_Month = month;
	RTC_DateTypeInitStructure.RTC_WeekDay = week;
	RTC_DateTypeInitStructure.RTC_Year = year;
    if(SUCCESS != RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure))
    {
        return D_ERROR_CODE_MCU_RTC_SET_DATA;
    }
	return D_ERROR_CODE_NONE;
}

//设置闹钟时间(按星期闹铃,24小时制)
void MCU_RTC_SetAlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	//关闭闹钟A 
	    RTC_AlarmCmd(RTC_Alarm_A,DISABLE);
	//设置时间
        RTC_TimeTypeInitStructure.RTC_Hours = hour;
        RTC_TimeTypeInitStructure.RTC_Minutes = min;
        RTC_TimeTypeInitStructure.RTC_Seconds = sec;
        RTC_TimeTypeInitStructure.RTC_H12 = RTC_H12_AM;
    //设置闹钟
	    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;
    //按星期闹
	    RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;
    //精确匹配星期，时分秒
        RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;
        RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
        RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
	//清除RTC闹钟A的标志
	    RTC_ClearITPendingBit(RTC_IT_ALRA);
    //清除LINE17上的中断标志位
        EXTI_ClearITPendingBit(EXTI_Line17); 
	//开启闹钟A中断
	    RTC_ITConfig(RTC_IT_ALRA,ENABLE);
    //开启闹钟A 
	    RTC_AlarmCmd(RTC_Alarm_A,ENABLE);
	//配置闹钟中断
        EXTI_InitStructure.EXTI_Line = EXTI_Line17;//LINE17
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发 
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE17
        EXTI_Init(&EXTI_InitStructure);//配置
    //配置NVIC中断向量
        NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MCU_RTC_ALARM_PREE_PRI;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = MCU_RTC_ALARM_SUB_PRI;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}	

//周期性唤醒定时器设置	
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
//配置WAKE UP中断,1秒钟中断一次
//MCU_RTC_SetWakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		
void MCU_RTC_SetWakeUp(uint32_t wksel,uint16_t cnt)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
    //关闭WAKE UP
	    RTC_WakeUpCmd(DISABLE);
    //唤醒时钟选择
	    RTC_WakeUpClockConfig(wksel);
    //设置WAKE UP自动重装载寄存器
	    RTC_SetWakeUpCounter(cnt);
    //清除RTC WAKE UP的标志
	    RTC_ClearITPendingBit(RTC_IT_WUT); 
    //清除LINE22上的中断标志位 
        EXTI_ClearITPendingBit(EXTI_Line22);
	//开启WAKE UP 定时器中断
	    RTC_ITConfig(RTC_IT_WUT,ENABLE);
    //开启WAKE UP 定时器　
	    RTC_WakeUpCmd( ENABLE);
	//中断线选择
        EXTI_InitStructure.EXTI_Line = EXTI_Line22;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
    //NVIC向量配置
        NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = MCU_RTC_WAKEUP_PREE_PRI;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = MCU_RTC_WAKEUP_SUB_PRI;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}	

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
    //ALARM A中断
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
        //Do Something Here

	}   
    //清除中断线17的中断标志 
	EXTI_ClearITPendingBit(EXTI_Line17);												 
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
    //WK_UP中断
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)
	{ 
        //清除中断标志
		RTC_ClearFlag(RTC_FLAG_WUTF);	
		//Do Something Here
	} 
    //清除中断线22的中断标志 	  
	EXTI_ClearITPendingBit(EXTI_Line22);							
}
