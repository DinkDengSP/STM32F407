/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:31:21
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-06 01:01:58
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_SDIO.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __MCU_SDIO_H_
#define __MCU_SDIO_H_
#include "MCU_SDIO_Base.h"

/*******************************用户配置区***********************************/		  
//SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz
//使用DMA模式的时候,传输速率可以到48Mhz(bypass on时),不过如果你的卡不是高速
//卡,可能也会出错,出错就请降低时钟
//SDIO初始化频率，最大400Kh 
#define SDIO_INIT_CLK_DIV        0x76 	
//SDIO传输频率,该值太小可能会导致读写文件出错
#define SDIO_TRANSFER_CLK_DIV    0x00	


//SDIO端口初始化
void MCU_SDIO_PortInit(void);

//SDIO时钟初始化设置 clkdiv:时钟分频系数 CK时钟=SDIOCLK/[clkdiv+2];(SDIOCLK时钟固定为48Mhz)
void MCU_SDIO_ClockSet(uint8_t clkdiv);

//SDIO发送指令
void MCU_SDIO_SendCommand(uint32_t argument,uint32_t cmdIndex,uint32_t respType,uint32_t waitInt,uint32_t cpsmState);

//设置数据传输模式
void MCU_SDIO_SetDataConfig(uint32_t dataBlockSize,uint32_t dataLength,uint32_t dataTimeOut,uint32_t dpsm,uint32_t transferDir,uint32_t transferMode);

//检查CMD0的执行状态
SD_ERROR MCU_SDIO_CheckCmd0Resp(void);

//检查R1响应的错误状态
SD_ERROR MCU_SDIO_CheckCmdR1Resp(uint8_t cmd);

//检查R2响应的错误状态
SD_ERROR MCU_SDIO_CheckCmdR2Resp(void);

//检查R3响应的错误状态
SD_ERROR MCU_SDIO_CheckCmdR3Resp(void);

//检查R6响应的错误状态
SD_ERROR MCU_SDIO_CheckCmdR6Resp(uint8_t cmd,uint16_t*prca);

//检查R7响应的错误状态
SD_ERROR MCU_SDIO_CheckCmdR7Resp(void);

//得到NumberOfBytes以2为底的指数.
uint8_t MCU_SDIO_ConvertFromBytesToPowerOfTwo(uint16_t NumberOfBytes);






#endif




