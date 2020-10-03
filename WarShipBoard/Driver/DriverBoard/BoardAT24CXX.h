/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:26:50
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 16:21:53
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardAT24CXX.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __BOARD_AT24CXX_H_
#define __BOARD_AT24CXX_H_
#include "MCU_Inc.h"

//芯片写地址
#define BOARD_AT24CXX_ADDR_WRITE        0XA0
//芯片读地址
#define BOARD_AT24CXX_ADDR_READ         (BOARD_AT24CXX_ADDR_WRITE+1)
//芯片写入间隔时间,at24CXX系列芯片的两次写入之间会有延时,ATMEL的标准是2MS,国产杂牌一般10MS以内
#define TIMS_MS_WRITE_AT24CXX_INTERVAL  10

//不同芯片的最大地址,同时可以区别芯片和芯片内存
#define MAX_ADDR_AT24C01        127
#define MAX_ADDR_AT24C02        255
#define MAX_ADDR_AT24C04        511
#define MAX_ADDR_AT24C08        1023
#define MAX_ADDR_AT24C16        2047
#define MAX_ADDR_AT24C32        4095
#define MAX_ADDR_AT24C64        8191
#define MAX_ADDR_AT24C128       16383
#define MAX_ADDR_AT24C256       32767
//设定本地芯片的类型
#define MAX_AT_ADDR_SELF        MAX_ADDR_AT24C02

//IIC芯片初始化
void BoardAT24CXX_PortInit(void);

//IIC芯片自检
D_ERROR_CODE BoardAT24CXX_SelfCheck(void);

//读取指定指定地址的一个数据
D_ERROR_CODE BoardAT24CXX_ReadOneByte(uint16_t addr, uint8_t *readDat);

//写入指定地址的一个数据
D_ERROR_CODE BoardAT24CXX_WriteOneByte(uint16_t addr, uint8_t writeDat);

//读取指定区间的数据
D_ERROR_CODE BoardAT24CXX_ReadBufferLength(uint16_t startAddr, uint16_t length, uint8_t *readBuffer);

//写入指定区间的数据
D_ERROR_CODE BoardAT24CXX_WriteBufferLength(uint16_t startAddr, uint16_t length, uint8_t *writeBuffer);






#endif





