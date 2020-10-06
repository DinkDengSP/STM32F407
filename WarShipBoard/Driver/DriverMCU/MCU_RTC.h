/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:20:32
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 13:17:28
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_RTC.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_RTC_H_
#define __MCU_RTC_H_
#include "MCU_Common.h"

//RTC初始化
D_ERROR_CODE MCU_RTC_Init(void);	
					
//RTC时间设置
D_ERROR_CODE MCU_RTC_SetTime(uint8_t hour,uint8_t min,uint8_t sec);

//RTC日期设置		
D_ERROR_CODE MCU_RTC_SetDate(uint8_t year,uint8_t month,uint8_t date,uint8_t week); 

//设置闹钟时间(按星期闹铃,24小时制)
void MCU_RTC_SetAlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec);	

//周期性唤醒定时器设置	
void MCU_RTC_SetWakeUp(u32 wksel,u16 cnt);					


#endif




