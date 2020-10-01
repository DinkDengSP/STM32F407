/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:16:48
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-01 23:55:37
 *FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\CommonFunc\SystemUtil\ErrorCode.h
 *ModifyRecord1:    
 *ModifyRecord2:    
 *ModifyRecord3:    
 *ModifyRecord4:    
 *ModifyRecord5:    
******************************************************************/
#ifndef __ERROR_CODE_H_
#define __ERROR_CODE_H_

//错误代码文件
typedef enum D_ERROR_CODE
{
    D_ERROR_CODE_NONE           = 0X00000000,//无错误
    D_ERROR_CODE_SRAM_CHECK     = 0X00000001,//板卡错误,SRAM校验错误
}D_ERROR_CODE;

#endif




