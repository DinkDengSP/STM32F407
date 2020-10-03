/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:16:48
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-03 16:01:27
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
    D_ERROR_CODE_NONE               = 0X00000000,//无错误
    D_ERROR_CODE_SRAM_CHECK         = 0X00000001,//板卡错误,SRAM校验错误
    D_ERROR_CODE_IIC_ACK_TIME_OUT   = 0X00000002,//板卡错误,IIC无返回
    D_ERROR_CODE_AT24CXX_CHECK      = 0X00000003,//AT24CXX自检错误
    D_ERROR_CODE_AT24CXX_ADDR_RANGE = 0X00000004,//AT24CXX地址错误
}D_ERROR_CODE;

#endif




