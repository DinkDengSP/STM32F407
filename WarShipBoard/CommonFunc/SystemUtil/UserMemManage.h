/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
 *Author: DengXiaoJun
 *Date: 2020-10-02 23:34:06
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 00:38:29
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\CommonFunc\SystemUtil\UserMemManage.h
 *ModifyRecord1:    
******************************************************************/
#ifndef __USER_MEM_MANAGE_H_
#define __USER_MEM_MANAGE_H_
#include "CoreInc.h"

//内存管理代码
typedef enum MEM_LOCATE
{
    MEM_SRAM_IN     = 0X00,//内部SRAM
    MEM_SRAM_CCM    = 0X01,//内部CCRAM
    MEM_SRAM_IS62   = 0X02,//外部SRAM
}MEM_LOCATE;

//设置内存块指定值
void UserMemSet(void *s,uint8_t c,uint32_t count);

//拷贝内存块
void UserMemCopy(void *des,void *src,uint32_t n);

//动态内存初始化
void UserMemInit(MEM_LOCATE locate);

//申请内存
void* UserMemMalloc(MEM_LOCATE locate,uint32_t size);

//释放内存
void UserMemFree(MEM_LOCATE locate,void* ptr);

//重新申请内存
void* UserMemRealloc(MEM_LOCATE locate,void *ptr,uint32_t size);

//获取当前动态内存使用率
uint8_t UserMemPerUsed(MEM_LOCATE locate);

#endif




