/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:23:17
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-01 23:45:18
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardSRAM.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_SRAM_H_
#define __BOARD_SRAM_H_
#include "MCU_Inc.h"

/**ISWV51216 512*16/2 1M字节*/
//对IS61LV25616/IS62WV25616,地址线范围为A0~A17 
//对IS61LV51216/IS62WV51216,地址线范围为A0~A18
#define BOARD_SRAM_BASE_ADDR    ((uint32_t)(0x68000000))
//1M容量
#define BOARD_SRAM_CAPACITY     (1024*1024)

//板上SRAM初始化
D_ERROR_CODE BoardSRAM_Init(void);

//板上SRAM自检
D_ERROR_CODE BoardSRAM_SelfCheck(void);

//板上SRAM写入数据
void BoardSRAM_WriteBuffer(uint32_t addr,uint32_t length,uint8_t* writeBufferPtr);

//板上SRAM读取数据
void BoardSRAM_ReadBuffer(uint32_t addr,uint32_t length,uint8_t* readBufferPtr);


#endif



