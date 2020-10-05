/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:25:08
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-05 13:15:37
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardW25QXX.c
**ModifyRecord1:    
******************************************************************/
#include "BoardW25QXX.h"

//读取FLASH ID
uint16_t  W25QXX_ReadID(void);  
//读取状态寄存器
uint8_t	 W25QXX_ReadSR(void); 
//写状态寄存器
uint8_t W25QXX_WriteSR(uint8_t sr);  
//写使能 
uint8_t W25QXX_WriteEnable(void);  
//写保护
uint8_t W25QXX_WriteDisable(void);
//芯片写入 无检查模式
uint8_t W25QXX_WriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
//等待空闲
uint8_t W25QXX_WaitBusy(void);    
//进入掉电模式
uint8_t W25QXX_PowerDown(void);   
//唤醒
uint8_t W25QXX_WAKEUP(void);	

//创建W25QXX访问信号量
OS_MUTEX mutex_W25QXX;

//定义CS
#define W25QXX_CS_ENABLE()      MCU_PortWriteSingle(MCU_PIN_B_14,Bit_RESET)
#define W25QXX_CS_RELEASE()     MCU_PortWriteSingle(MCU_PIN_B_14,Bit_SET)

//初始化,返回值为芯片ID
void W25QXX_Init(void)
{
	OS_ERR err;
	//创建一个互斥信号量,用于flash读写
    OSMutexCreate((OS_MUTEX*	)&mutex_W25QXX,
				  (CPU_CHAR*	)"mutex_W25QXX",
                  (OS_ERR*		)&err);
	//CS端口初始化
    MCU_PortInit(MCU_PIN_B_14, GPIO_Mode_OUT, GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_100MHz);
	//SPI FLASH不选中
	W25QXX_CS_RELEASE();	
	//初始化SPI,4分频84/4=21M
	MCU_SPI1_Init(MCU_SPI_SPEED_DIV4,MCU_SPI_CPOL_HIGH,MCU_SPI_CPHA_2EDGE);		  
	//设置为21M时钟,高速模式
	MCU_SPI1_SetSpeed(MCU_SPI_SPEED_DIV4);
}

//芯片自检
D_ERROR_CODE W25QXX_SelfCheck(void)
{
    //读取芯片ID
    uint16_t chipID = W25QXX_ReadID();
    //ID匹配
    if(chipID == W25QXX_TYPE)
    {
        //匹配成功,尝试读写一次
        uint32_t selfCheckAddr = MCU_RandomGetNextRangeReal(0,W25QXX_ADDR_MAX-1);
        uint8_t readByte1 = 0,writeByte = 0,readByte2 = 0;
        //读取数据
        W25QXX_Read(&readByte1,selfCheckAddr,1);
        //数据计算
        writeByte = (~readByte1);
        //写入数据
        W25QXX_Write(&writeByte,selfCheckAddr,1);
        //再次读取
        W25QXX_Read(&readByte2,selfCheckAddr,1);
        //看看数据是否匹配
        if(readByte2 != writeByte)
        {
            return D_ERROR_CODE_W25QXX_RW_CHECK;
        }
        //将原始数据写入
        W25QXX_Write(&readByte1,selfCheckAddr,1);
        //读取原始数据
        W25QXX_Read(&readByte2,selfCheckAddr,1);
        //看看数据是否匹配
        if(readByte1 != readByte2)
        {
            return D_ERROR_CODE_W25QXX_RW_CHECK;
        }
        //返回成功
        return D_ERROR_CODE_NONE;
    }
    else
    {
        return D_ERROR_CODE_W25QXX_ID_CHECK;
    }
}

//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 
uint16_t  W25QXX_ReadID(void)
{
	uint16_t Temp = 0;
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	W25QXX_CS_ENABLE();		
	//发送读取ID命令	
	MCU_SPI1_WriteRead(0x90);    
	MCU_SPI1_WriteRead(0x00); 	    
	MCU_SPI1_WriteRead(0x00); 	    
	MCU_SPI1_WriteRead(0x00); 	 			   
	Temp|=MCU_SPI1_WriteRead(0xFF)<<8;  
	Temp|=MCU_SPI1_WriteRead(0xFF);	 
	W25QXX_CS_RELEASE();	
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	return Temp;	
}	

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t	W25QXX_ReadSR(void)
{
	uint8_t byte=0; 
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件 
	W25QXX_CS_ENABLE();             
	//发送读取状态寄存器命令 
	MCU_SPI1_WriteRead(CMD_W25X_ReadStatusReg);     
	//读取一个字节
	byte=MCU_SPI1_WriteRead(0Xff);     
	//取消片选  
	W25QXX_CS_RELEASE();
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	return byte;
}

//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
uint8_t W25QXX_WriteSR(uint8_t sr)
{
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件
	W25QXX_CS_ENABLE();             
	//发送写取状态寄存器命令
	MCU_SPI1_WriteRead(CMD_W25X_WriteStatusReg);
	//写入一个字节
	MCU_SPI1_WriteRead(sr);
	//取消片选 	
	W25QXX_CS_RELEASE();
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);	
	return 0;
}

//W25QXX写使能	
//将WEL置位   
uint8_t W25QXX_WriteEnable(void)
{
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件
	W25QXX_CS_ENABLE();         
	//发送写使能
    MCU_SPI1_WriteRead(CMD_W25X_WriteEnable); 
	//取消片选
	W25QXX_CS_RELEASE(); 
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	return 0;
}	

//W25QXX写禁止	
//将WEL清零  
uint8_t W25QXX_WriteDisable(void)
{
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件 
	W25QXX_CS_ENABLE();   
	//发送写禁止指令	
    MCU_SPI1_WriteRead(CMD_W25X_WriteDisable);  
	//取消片选
	W25QXX_CS_RELEASE(); 
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	return 0;
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
uint8_t W25QXX_WritePage(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
 	uint16_t i;  
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//SET WEL
    W25QXX_WriteEnable();
	//使能器件  	
	W25QXX_CS_ENABLE(); 
	//发送写页命令 
    MCU_SPI1_WriteRead(CMD_W25X_PageProgram); 
	//发送24bit地址
    MCU_SPI1_WriteRead((uint8_t)((WriteAddr)>>16));     
    MCU_SPI1_WriteRead((uint8_t)((WriteAddr)>>8));   
    MCU_SPI1_WriteRead((uint8_t)WriteAddr);   
	//循环写数
    for(i=0;i<NumByteToWrite;i++)
		MCU_SPI1_WriteRead(pBuffer[i]);
	//取消片选
	W25QXX_CS_RELEASE();
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err); 
    //等待写入结束
	W25QXX_WaitBusy();
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return 0;
} 

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
uint8_t W25QXX_WriteNoCheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint16_t pageremain;	
    OS_ERR err; 
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);  
	//单页剩余的字节数
	pageremain=256-WriteAddr%256; 		 	    
	//不大于256个字节
	if(NumByteToWrite<=pageremain)
        pageremain=NumByteToWrite;
	while(1)
	{	   
		W25QXX_WritePage(pBuffer,WriteAddr,pageremain);
		//写入结束了
		if(NumByteToWrite==pageremain)
			break;
		//NumByteToWrite>pageremain
	 	else 
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			//减去已经写入了的字节数
			NumByteToWrite-=pageremain;	
			//一次可以写入256个字节
			if(NumByteToWrite>256)
				pageremain=256; 
			//不够256个字节了
			else 
				pageremain=NumByteToWrite; 	  
		}
	}
    //释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return 0;
}

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
D_ERROR_CODE W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
	uint16_t i;
	OS_ERR err;
    //地址检查
    if(ReadAddr + NumByteToRead >= W25QXX_ADDR_MAX)
    {
        return D_ERROR_CODE_W25QXX_ADDR_RANGE;
    }
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件 
	W25QXX_CS_ENABLE();            
	//发送读取命令
	MCU_SPI1_WriteRead(CMD_W25X_ReadData);  
	//发送24bit地址 
	MCU_SPI1_WriteRead((uint8_t)((ReadAddr)>>16));     
	MCU_SPI1_WriteRead((uint8_t)((ReadAddr)>>8));   
	MCU_SPI1_WriteRead((uint8_t)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		//循环读数
		pBuffer[i]=MCU_SPI1_WriteRead(0XFF);     
	}
	W25QXX_CS_RELEASE();
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}



//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   
uint8_t W25QXX_BUFFER[4096];
D_ERROR_CODE W25QXX_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	uint8_t * W25QXX_BUF;	  
	OS_ERR err;
    //地址检查
    if(WriteAddr + NumByteToWrite >= W25QXX_ADDR_MAX)
    {
        return D_ERROR_CODE_W25QXX_ADDR_RANGE;
    }
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	W25QXX_BUF=W25QXX_BUFFER;	
	//扇区地址 
 	secpos=WriteAddr/4096; 
	//在扇区内的偏移
	secoff=WriteAddr%4096;
	//扇区剩余空间大小
	secremain=4096-secoff;   
	//不大于4096个字节
 	if(NumByteToWrite<=secremain)
		secremain=NumByteToWrite;
	while(1) 
	{	
		//读出整个扇区的内容
		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);
		//校验数据
		for(i=0;i<secremain;i++)
		{
			//需要擦除  
			if(W25QXX_BUF[secoff+i]!=0XFF)
				break;	  
		}
		//需要擦除
		if(i<secremain)
		{
			//擦除这个扇区
			W25QXX_EraseSector(secpos);
			//复制
			for(i=0;i<secremain;i++)	   
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			//写入整个扇区
			W25QXX_WriteNoCheck(W25QXX_BUF,secpos*4096,4096);  

		}
		//写已经擦除了的,直接写入扇区剩余区间. 
		else 
			W25QXX_WriteNoCheck(pBuffer,WriteAddr,secremain);	
		//写入结束了
		if(NumByteToWrite==secremain)
			break;
		//写入未结束
		else
		{
			//扇区地址增1
			secpos++;
			//偏移位置为0
			secoff=0; 	 

			//指针偏移
		   	pBuffer+=secremain; 
			//写地址偏移			
			WriteAddr+=secremain;	
			//字节数递减
		   	NumByteToWrite-=secremain;	
			//下一个扇区还是写不完
			if(NumByteToWrite>4096)
				secremain=4096;	
			//下一个扇区可以写完了
			else 
				secremain=NumByteToWrite;			
		}	 
	}
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}

//擦除整个芯片		  
//等待时间超长..
D_ERROR_CODE W25QXX_EraseChip(void)
{
	OS_ERR err;
    //申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//SET WEL
	W25QXX_WriteEnable();                   
    W25QXX_WaitBusy(); 
	//使能器件
  	W25QXX_CS_ENABLE();                     
	//发送片擦除命令
    MCU_SPI1_WriteRead(CMD_W25X_ChipErase);  
	//取消片选 
	W25QXX_CS_RELEASE();  
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	//等待芯片擦除结束
	W25QXX_WaitBusy();
	//释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}	

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
D_ERROR_CODE W25QXX_EraseSector(uint32_t Dst_Addr)
{
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);  
 	Dst_Addr*=4096;
	//SET WEL 
    W25QXX_WriteEnable();                  	 
    W25QXX_WaitBusy(); 
	//使能器件
  	W25QXX_CS_ENABLE();        
	//发送扇区擦除指令
    MCU_SPI1_WriteRead(CMD_W25X_SectorErase); 
	//发送24bit地址
    MCU_SPI1_WriteRead((uint8_t)((Dst_Addr)>>16));      
    MCU_SPI1_WriteRead((uint8_t)((Dst_Addr)>>8));   
    MCU_SPI1_WriteRead((uint8_t)Dst_Addr); 
	//取消片选 
	W25QXX_CS_RELEASE(); 
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	//等待擦除完成
    W25QXX_WaitBusy();  
    //释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return D_ERROR_CODE_NONE;
}

//等待空闲
uint8_t W25QXX_WaitBusy(void)
{
	// 等待BUSY位清空
	while((W25QXX_ReadSR()&0x01)==0x01)
	{
		CoreDelayMs(5);
	}		
	return 0;
}	

//进入掉电模式
uint8_t W25QXX_PowerDown(void)
{
	//禁用调度器
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件
	W25QXX_CS_ENABLE();         
	//发送掉电命令
	MCU_SPI1_WriteRead(CMD_W25X_PowerDown); 
	//取消片选
	W25QXX_CS_RELEASE();
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	//等待TPD	
	CoreDelayMs(1);   
    //释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return 0;
}	

//唤醒
uint8_t W25QXX_WAKEUP(void)
{
	//禁用调度器
	OS_ERR err;
	//申请总线信号量与IIC总线信号量
    //请求互斥信号量,申请不到,任务挂起等待OS_OPT_PEND_BLOCKING
    if (OSRunning)OSMutexPend(&mutex_W25QXX, 0, OS_OPT_PEND_BLOCKING, 0, &err);
    if (OSRunning)OSMutexPend(&mutexMCU_SPI1, 0, OS_OPT_PEND_BLOCKING, 0, &err);
	//使能器件
	W25QXX_CS_ENABLE();  
	//send CMD_W25X_PowerDown command 0xAB 
	MCU_SPI1_WriteRead(CMD_W25X_ReleasePowerDown);  
	//取消片选
	W25QXX_CS_RELEASE();
    //释放信号量
    if (OSRunning)OSMutexPost(&mutexMCU_SPI1, OS_OPT_POST_FIFO, &err);
	//等待TRES1
	CoreDelayMs(1);     
    //释放信号量
    if (OSRunning)OSMutexPost(&mutex_W25QXX, OS_OPT_POST_FIFO, &err);
	return 0;	
}

