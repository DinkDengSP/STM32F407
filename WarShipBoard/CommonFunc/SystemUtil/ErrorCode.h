/*******************************************************************
 *Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved
 *Author: DengXiaoJun
 *Date: 2020-09-26 14:16:48
 *LastEditors: DengXiaoJun
 *LastEditTime: 2020-10-05 14:07:47
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
    D_ERROR_CODE_EXTI_LINE_RANGE    = 0X00000005,//外部中断线范围错误
    D_ERROR_CODE_EXTI_USED_EXIST    = 0X00000006,//想要设置的外部中断已经在使用中
    D_ERROR_CODE_EXTI_CALLBACK_NULL = 0X00000007,//中断回调函数为空
    D_ERROR_CODE_MPU6050_ID_CHECK   = 0X00000008,//MPU6050芯片ID检测失败
    D_ERROR_CODE_W25QXX_ID_CHECK    = 0X00000009,//W25QXX芯片ID检测失败
    D_ERROR_CODE_W25QXX_ADDR_RANGE  = 0X0000000A,//W25QXX芯片的地址错误
    D_ERROR_CODE_W25QXX_RW_CHECK    = 0X0000000B,//W25QXX芯片读写失败
    D_ERROR_CODE_MCU_FLASH_ADDR     = 0X0000000C,//flash地址不合标准
    D_ERROR_CODE_MCU_FLASH_ERASE    = 0X0000000D,//Flash擦除错误
    D_ERROR_CODE_MCU_FLASH_WRITE    = 0X0000000E,//flash写入失败
}D_ERROR_CODE;

#endif




