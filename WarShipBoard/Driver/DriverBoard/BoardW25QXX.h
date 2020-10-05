/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:25:15
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 13:07:59
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardW25QXX.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_W25QXX_H_
#define __BOARD_W25QXX_H_
#include "BoardW25QXXBase.h"

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128 容量为16M字节,共有128个Block,4096个Sector 
#define W25QXX_TYPE	        CHIP_ID_W25Q128		
#define W25QXX_ADDR_MAX     (16*1024*1024)         

//芯片初始化
void W25QXX_Init(void);

//芯片自检
D_ERROR_CODE W25QXX_SelfCheck(void);

//读取flash
D_ERROR_CODE W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead); 

//写入flash
D_ERROR_CODE W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);

//整片擦除
D_ERROR_CODE W25QXX_EraseChip(void);    	 

//扇区擦除
D_ERROR_CODE W25QXX_EraseSector(uint32_t Dst_Addr);



#endif



