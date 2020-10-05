/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-05 11:09:43
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 11:15:52
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardW25QXXBase.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_W25QXX_BASE_H_
#define __BOARD_W25QXX_BASE_H_
#include "MCU_Inc.h"

//芯片识别号
#define CHIP_ID_W25Q80 	0XEF13 	
#define CHIP_ID_W25Q16 	0XEF14
#define CHIP_ID_W25Q32 	0XEF15
#define CHIP_ID_W25Q64 	0XEF16
#define CHIP_ID_W25Q128	0XEF17

//指令表
#define CMD_W25X_WriteEnable		0x06 
#define CMD_W25X_WriteDisable		0x04 
#define CMD_W25X_ReadStatusReg		0x05 
#define CMD_W25X_WriteStatusReg		0x01 
#define CMD_W25X_ReadData			0x03 
#define CMD_W25X_FastReadData		0x0B 
#define CMD_W25X_FastReadDual		0x3B 
#define CMD_W25X_PageProgram		0x02 
#define CMD_W25X_BlockErase			0xD8 
#define CMD_W25X_SectorErase		0x20 
#define CMD_W25X_ChipErase			0xC7 
#define CMD_W25X_PowerDown			0xB9 
#define CMD_W25X_ReleasePowerDown	0xAB 
#define CMD_W25X_DeviceID			0xAB 
#define CMD_W25X_ManufactDeviceID	0x90 
#define CMD_W25X_JedecDeviceID		0x9F 

#endif

