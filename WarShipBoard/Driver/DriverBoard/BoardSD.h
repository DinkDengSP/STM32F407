/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:31:39
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 01:21:42
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardSD.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_SD_H_
#define __BOARD_SD_H_
#include "BoardSDBase.h"

//是否使能SD卡调试功能
#define SD_SDIO_USE_DEBUG		1

#if (SD_SDIO_USE_DEBUG == 1)
#include "MCU_Uart.h"
#define sd_sdio_debug_printf(format,args...)	MCU_Uart1Printf(format,##args)		//变参宏定义
#else
#define  sd_sdio_debug_printf(x,...)  while(0);
#endif

extern SD_CardInfo SDCardInfo;//SD卡信息

//端口初始化
void BoardSD_PortInit(void);

//初始化SD卡
//返回值:错误代码;(0,无错误)
SD_ERROR BoardSD_Init(void);

//读SD卡 buf:读数据缓存区 sector:扇区地址 cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;				  				 
uint8_t BoardSD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);

//写SD卡 buf:写数据缓存区 sector:扇区地址 cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
uint8_t BoardSD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);


//打印全部SD卡信息
void ConsoleSendCardMessage(void);

#endif



