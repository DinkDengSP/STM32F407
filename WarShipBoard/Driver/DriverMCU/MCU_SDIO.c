/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaoJun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:31:13
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 00:01:15
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverMCU\MCU_SDIO.c
**ModifyRecord1:    
**ModifyRecord2:    
**ModifyRecord3:    
**ModifyRecord4:    
**ModifyRecord5:    
******************************************************************/
#include "MCU_SDIO.h"
#include "MCU_Port.h"

//SDIO外设寄存器复位为初始值
static void MCU_SDIO_Register_Deinit(void)
{
	SDIO->POWER=0x00000000;
	SDIO->CLKCR=0x00000000;
	SDIO->ARG=0x00000000;
	SDIO->CMD=0x00000000;
	SDIO->DTIMER=0x00000000;
	SDIO->DLEN=0x00000000;
	SDIO->DCTRL=0x00000000;
	SDIO->ICR=0x00C007FF;
	SDIO->MASK=0x00000000;	 
}

	
//SDIO端口初始化
void MCU_SDIO_PortInit(void)
{
    //SDIO时钟使能
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
    //SDIO复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);
    //SDIO结束复位
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);
    //PC8 9 10 11 12复用输出
    //D0 C8
    MCU_PortInit(MCU_PIN_C_8, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //D1 C9
    MCU_PortInit(MCU_PIN_C_9, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //D2 C10
    MCU_PortInit(MCU_PIN_C_10, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //D3 C11
    MCU_PortInit(MCU_PIN_C_11, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //SCK PC12
    MCU_PortInit(MCU_PIN_C_12, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //PD2复用输出 CMD
    MCU_PortInit(MCU_PIN_D_2, GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_100MHz);
    //引脚复用映射设置 PC8,AF12
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);	
    //SDIO外设寄存器设置为默认值 			   
	MCU_SDIO_Register_Deinit();
}

//SDIO时钟初始化设置 clkdiv:时钟分频系数 CK时钟=SDIOCLK/[clkdiv+2];(SDIOCLK时钟固定为48Mhz)
void MCU_SDIO_ClockSet(uint8_t clkdiv)
{
	uint32_t tmpreg=SDIO->CLKCR; 
	tmpreg&=0XFFFFFF00; 
	tmpreg|=clkdiv;   
	SDIO->CLKCR=tmpreg;
} 

//SDIO发送指令
void MCU_SDIO_SendCommand(uint32_t argument,uint32_t cmdIndex,uint32_t respType,uint32_t waitInt,uint32_t cpsmState)
{
    SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
    SDIO_CmdInitStructure.SDIO_Argument = argument;
    SDIO_CmdInitStructure.SDIO_CmdIndex = cmdIndex;
    SDIO_CmdInitStructure.SDIO_Response = respType;  
    SDIO_CmdInitStructure.SDIO_Wait = waitInt;
    SDIO_CmdInitStructure.SDIO_CPSM = cpsmState; 
    //写命令进命令寄存器
    SDIO_SendCommand(&SDIO_CmdInitStructure);
}

//设置数据传输模式
void MCU_SDIO_SetDataConfig(uint32_t dataBlockSize,uint32_t dataLength,uint32_t dataTimeOut,uint32_t dpsm,uint32_t transferDir,uint32_t transferMode)
{
    SDIO_DataInitTypeDef SDIO_DataInitStructure; 
    SDIO_DataInitStructure.SDIO_DataBlockSize = dataBlockSize; ;		
    SDIO_DataInitStructure.SDIO_DataLength = dataLength ;
    SDIO_DataInitStructure.SDIO_DataTimeOut = dataTimeOut ;
    SDIO_DataInitStructure.SDIO_DPSM =dpsm;
    SDIO_DataInitStructure.SDIO_TransferDir = transferDir;
    SDIO_DataInitStructure.SDIO_TransferMode = transferMode;
    SDIO_DataConfig(&SDIO_DataInitStructure);
}

//检查CMD0的执行状态
//返回值:sd卡错误码
SD_ERROR MCU_SDIO_CheckCmd0Resp(void)
{
	SD_ERROR errorstatus = SD_OK;
	uint32_t timeout=SDIO_CMD0TIMEOUT;	   
	while(timeout--)
	{
		//命令已发送(无需响应)	
		if(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)
			break;	 
	}	    
	if(timeout==0)
		return SD_CMD_RSP_TIMEOUT;
	//清除所有标记	
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	return errorstatus;
}	 

//检查R1响应的错误状态
//cmd:当前命令
//返回值:sd卡错误码
SD_ERROR MCU_SDIO_CheckCmdR1Resp(uint8_t cmd)
{	  
	uint32_t status; 
	while(1)
	{
		status=SDIO->STA;
		//CRC错误/命令响应超时/已经收到响应(CRC校验成功)
		if(status&((1<<0)|(1<<2)|(1<<6)))
			break;
	} 
	//响应超时
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					
	{				
		//清除命令响应超时标志
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 				
		return SD_CMD_RSP_TIMEOUT;
	}	
	//CRC错误
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)					
	{					
		//清除标志
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL); 				
		return SD_CMD_CRC_FAIL;
	}		
	//命令不匹配 
	if(SDIO->RESPCMD!=cmd)
		return SD_ILLEGAL_CMD;
	//清除所有标记
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	//返回卡响应
	return (SD_ERROR)(SDIO->RESP1&SD_OCR_ERRORBITS);
}

//检查R2响应的错误状态
//返回值:错误状态
SD_ERROR MCU_SDIO_CheckCmdR2Resp(void)
{
	SD_ERROR errorstatus=SD_OK;
	uint32_t status;
	uint32_t timeout=SDIO_CMD0TIMEOUT;
	while(timeout--)
	{
		status=SDIO->STA;
		//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
		if(status&((1<<0)|(1<<2)|(1<<6)))
			break;
	}
	//响应超时
	if((timeout==0)||(status&(1<<2)))	
	{																				    
		errorstatus=SD_CMD_RSP_TIMEOUT;
		//清除命令响应超时标志		
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 		
		return errorstatus;
	}	 
	//CRC错误
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						
	{								   
		errorstatus=SD_CMD_CRC_FAIL;
		//清除响应标志
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);		
	}
	//清除所有标记
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	return errorstatus;								    		 
} 

//检查R3响应的错误状态
//返回值:错误状态
SD_ERROR MCU_SDIO_CheckCmdR3Resp(void)
{
	uint32_t status;						 
	while(1)
	{
		status=SDIO->STA;
		//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
		if(status&((1<<0)|(1<<2)|(1<<6)))
			break;
	}
	//响应超时
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					
	{			
		//清除命令响应超时标志
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			
		return SD_CMD_RSP_TIMEOUT;
	}	 
	//清除所有标记
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	return SD_OK;								  
}

//检查R6响应的错误状态
//cmd:之前发送的命令
//prca:卡返回的RCA地址
//返回值:错误状态
SD_ERROR MCU_SDIO_CheckCmdR6Resp(uint8_t cmd,uint16_t*prca)
{
	SD_ERROR errorstatus=SD_OK;
	uint32_t status;					    
	uint32_t rspr1;
	while(1)
	{
		status=SDIO->STA;
		//CRC错误/命令响应超时/已经收到响应(CRC校验成功)	
		if(status&((1<<0)|(1<<2)|(1<<6)))break;
	}
	//响应超时
	if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					
	{				
		//清除命令响应超时标志
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			
		return SD_CMD_RSP_TIMEOUT;
	}	 	 
	//CRC错误
	if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						
	{		
		//清除响应标志		
		SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);					
		return SD_CMD_CRC_FAIL;
	}
	//判断是否响应cmd命令
	if(SDIO->RESPCMD!=cmd)				
	{
		return SD_ILLEGAL_CMD; 		
	}	    
	//清除所有标记
	SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	//得到响应
	rspr1=SDIO->RESP1;					 	 
	if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
	{
		//右移16位得到,rca
		*prca=(uint16_t)(rspr1>>16);			
		return errorstatus;
	}
	if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)
		return SD_GENERAL_UNKNOWN_ERROR;
	if(rspr1&SD_R6_ILLEGAL_CMD)
		return SD_ILLEGAL_CMD;
	if(rspr1&SD_R6_COM_CRC_FAILED)
		return SD_COM_CRC_FAILED;
	return errorstatus;
}

//检查R7响应的错误状态
//返回值:sd卡错误码
SD_ERROR MCU_SDIO_CheckCmdR7Resp(void)
{
	SD_ERROR errorstatus=SD_OK;
	uint32_t status;
	uint32_t timeout=SDIO_CMD0TIMEOUT;
	while(timeout--)
	{
		status=SDIO->STA;
		//CRC错误/命令响应超时/已经收到响应(CRC校验成功)
		if(status&((1<<0)|(1<<2)|(1<<6)))
			break;	
	}
	//响应超时
	if((timeout==0)||(status&(1<<2)))	
	{					
		//当前卡不是2.0兼容卡,或者不支持设定的电压范围
		errorstatus=SD_CMD_RSP_TIMEOUT;	
		//清除命令响应超时标志
		SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 			
		return errorstatus;
	}
//成功接收到响应	
	if(status&1<<6)						
	{								   
		errorstatus=SD_OK;
		//清除响应标志
		SDIO_ClearFlag(SDIO_FLAG_CMDREND); 				
	}
	return errorstatus;
}	

//得到NumberOfBytes以2为底的指数.
//NumberOfBytes:字节数.
//返回值:以2为底的指数值
uint8_t MCU_SDIO_ConvertFromBytesToPowerOfTwo(uint16_t NumberOfBytes)
{
	uint8_t count=0;
	while(NumberOfBytes!=1)
	{
		NumberOfBytes>>=1;
		count++;
	}
	return count;
} 	 










