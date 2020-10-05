/*******************************************************************
**Description: Copyright(c) 2018-2090 DengXiaojun,All rights reserved.
**Author: DengXiaoJun
**Date: 2020-09-26 14:31:30
**LastEditors: DengXiaoJun
**LastEditTime: 2020-10-06 01:31:34
**FilePath: \HardWareCheckUCOS3.08d:\DinkGitHub\STM32F407\WarShipBoard\Driver\DriverBoard\BoardSD.c
**ModifyRecord1:    
******************************************************************/
#include "BoardSD.h"
#include "string.h"

//SD卡类型（默认为1.x卡）
static uint8_t CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;	
//SD卡CSD,CID以及相对地址(RCA)数据
static uint32_t CSD_Tab[4],CID_Tab[4],RCA=0;		
//是否发送停止传输标志位,DMA多块读写的时候用到  
static uint8_t StopCondition=0; 		
//数据传输错误标志,DMA读写时使用
volatile SD_ERROR TransferError=SD_OK;
//传输结束标志,DMA读写时使用
volatile uint8_t TransferEnd=0;	
//SD卡信息
SD_CardInfo SDCardInfo;	
//SD_ReadDisk/SD_WriteDisk函数专用buf,当这两个函数的数据缓存区地址不是4字节对齐的时候,
//需要用到该数组,确保数据缓存区地址是4字节对齐的.
uint8_t SDIO_DATA_BUFFER[512]__attribute__((aligned(4)));
//SD卡访问信号量
OS_MUTEX mutexBoard_SDCARD;

//SD卡 Power OFF
SD_ERROR BoardSD_PowerOFF(void)
{
	//SDIO电源关闭,时钟停止
	SDIO_SetPowerState(SDIO_PowerState_OFF);	
	return SD_OK;	  
}  


//卡上电,查询所有SDIO接口上的卡设备,并查询其电压和配置时钟
SD_ERROR BoardSD_PowerON(void)
{
    //初始化需要的变量
        SD_ERROR errorstatus=SD_OK;
        uint32_t response=0,count=0,validvoltage=0;
        uint32_t SDType=SD_STD_CAPACITY;
        /*用于sdio初始化的结构体*/
        SDIO_InitTypeDef SDIO_InitStructure;
	/*初始化时的时钟不能大于400KHz*/ 
        /* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
        SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;	
        SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
        //不使用bypass模式，直接用HCLK进行分频得到SDIO_CK
        SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable; 
        // 空闲时不关闭时钟电源	
        SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
        //1位数据线
        SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;	 
        //硬件流
        SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
        SDIO_Init(&SDIO_InitStructure);
	//上电状态,开启卡时钟 
        SDIO_SetPowerState(SDIO_PowerState_ON);
        //SDIOCK使能 
        SDIO->CLKCR|=1<<8;			 
    //首先卡进入空闲模式
        for(uint8_t indexUtil=0 ;indexUtil<74 ;indexUtil++)
        {
            //发送CMD0进入IDLE STAGE模式命令.无响应,则CPSM在开始发送命令之前等待数据传输结束。 	
            MCU_SDIO_SendCommand(0x0,SD_CMD_GO_IDLE_STATE,SDIO_Response_No,SDIO_Wait_No,SDIO_CPSM_Enable);	
            errorstatus=MCU_SDIO_CheckCmd0Resp();
            if(errorstatus==SD_OK)
                break;
        }
        //返回错误状态
        if(errorstatus)return errorstatus;
    //发送CMD8,短响应,检查SD卡接口特性,关闭等待中断
        MCU_SDIO_SendCommand(SD_CHECK_PATTERN,SDIO_SEND_IF_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        //等待R7响应
        errorstatus=MCU_SDIO_CheckCmdR7Resp();						
    //R7响应正常
        if(errorstatus==SD_OK) 								
        {
            //SD 2.0卡
            CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;
            //高容量卡
            SDType=SD_HIGH_CAPACITY;			   			
        }
    //发送CMD55,短响应		
        MCU_SDIO_SendCommand(0x00,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);	 
        //等待R1响应  
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_CMD); 		 	 
	//SD2.0/SD 1.1,否则为MMC卡
        if(errorstatus==SD_OK)
        {																  
            //SD卡,发送ACMD41 SD_APP_OP_COND,参数为:0x80100000 
            while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
            {	   						
                //发送CMD55,短响应		
                    MCU_SDIO_SendCommand(0x00,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);				 
                    //等待R1响应
                    errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_CMD); 	 	  
                    //响应错误
                    if(errorstatus!=SD_OK)return errorstatus;   	
                //acmd41，命令参数由支持的电压范围及HCS位组成，HCS位置一来区分卡是SDSc还是sdhc
                    //发送ACMD41,短响应
                    MCU_SDIO_SendCommand(SD_VOLTAGE_WINDOW_SD|SDType,SD_CMD_SD_APP_OP_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                    //等待R3响应
                    errorstatus=MCU_SDIO_CheckCmdR3Resp(); 					   
                    //响应错误 
                    if(errorstatus!=SD_OK)return errorstatus; 
                //得到响应
                    response=SDIO->RESP1;
                //判断SD卡上电是否完成
                    validvoltage=(((response>>31)==1)?1:0);			
                    count++;
            }
            if(count>=SD_MAX_VOLT_TRIAL)
            {
                errorstatus=SD_INVALID_VOLTRANGE;
                return errorstatus;
            }	 
            if(response&=SD_HIGH_CAPACITY)
            {
                CardType=SDIO_HIGH_CAPACITY_SD_CARD;
            }
        }
	//MMC卡
        else
        {
            //MMC卡,发送CMD1 SDIO_SEND_OP_COND,参数为:0x80FF8000 
            while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
            {	   			
                //发送CMD1,短响应
                    MCU_SDIO_SendCommand(SD_VOLTAGE_WINDOW_MMC,SD_CMD_SEND_OP_COND,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                    //等待R3响应
                    errorstatus=MCU_SDIO_CheckCmdR3Resp(); 					   
                    //响应错误
                    if(errorstatus!=SD_OK)return errorstatus; 
                //得到响应
                    response=SDIO->RESP1;;			   				
                    validvoltage=(((response>>31)==1)?1:0);
                    count++;
            }
            if(count>=SD_MAX_VOLT_TRIAL)
            {
                errorstatus=SD_INVALID_VOLTRANGE;
                return errorstatus;
            }	 			    
            CardType=SDIO_MULTIMEDIA_CARD;	  
        }  
	return(errorstatus);		
}


//初始化所有的卡,并让卡进入就绪状态
SD_ERROR BoardSD_InitializeCards(void)
{
	SD_ERROR errorstatus=SD_OK;
	uint16_t rca = 0x01;
	//检查电源状态,确保为上电状态
        if (SDIO_GetPowerState() == SDIO_PowerState_OFF)	
        {
            errorstatus = SD_REQUEST_NOT_APPLICABLE;
            return(errorstatus);
        }
	//非SECURE_DIGITAL_IO_CARD
        if(SDIO_SECURE_DIGITAL_IO_CARD != CardType)			
        {
            //发送CMD2,取得CID,长响应
                MCU_SDIO_SendCommand(0x0,SD_CMD_ALL_SEND_CID,SDIO_Response_Long,SDIO_Wait_No,SDIO_CPSM_Enable);
                //等待R2响应
                errorstatus=MCU_SDIO_CheckCmdR2Resp(); 					 
                //响应错误
                if(errorstatus!=SD_OK)return errorstatus; 
            //记录CID数据
                CID_Tab[0]=SDIO->RESP1;
                CID_Tab[1]=SDIO->RESP2;
                CID_Tab[2]=SDIO->RESP3;
                CID_Tab[3]=SDIO->RESP4;
        }
	//判断卡类型,不同的卡设置地址的方式不同
        if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)|| \
            (SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
        {
            //发送CMD3,短响应 
            MCU_SDIO_SendCommand(0x00,SD_CMD_SET_REL_ADDR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            //等待R6响应
            errorstatus=MCU_SDIO_CheckCmdR6Resp(SD_CMD_SET_REL_ADDR,&rca); 
            //响应错误
            if(errorstatus!=SD_OK)return errorstatus; 		    
        }   
    //多媒体卡
        if (SDIO_MULTIMEDIA_CARD==CardType)
        {
            //发送CMD3,短响应 
            MCU_SDIO_SendCommand((uint32_t)(rca<<16),SD_CMD_SET_REL_ADDR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);	
            //等待R2响应 
            errorstatus=MCU_SDIO_CheckCmdR2Resp(); 					  
            //响应错误
            if(errorstatus!=SD_OK)return errorstatus; 		 
        }
	//非SECURE_DIGITAL_IO_CARD
        if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			
        {
            RCA = rca;
            //发送CMD9+卡RCA,取得CSD,长响应
                MCU_SDIO_SendCommand((uint32_t)(rca << 16),SD_CMD_SEND_CSD,SDIO_Response_Long,SDIO_Wait_No,SDIO_CPSM_Enable);
                //等待R2响应 
                errorstatus=MCU_SDIO_CheckCmdR2Resp(); 
                //响应错误
                if(errorstatus!=SD_OK)return errorstatus;
            //记录CSD       		    	    
                CSD_Tab[0]=SDIO->RESP1;
                CSD_Tab[1]=SDIO->RESP2;
                CSD_Tab[2]=SDIO->RESP3;						
                CSD_Tab[3]=SDIO->RESP4;					    
        }
	return SD_OK;//卡初始化成功
} 


//得到卡信息cardinfo:卡信息存储区
SD_ERROR BoardSD_GetCardInfo(SD_CardInfo *cardinfo)
{
	SD_ERROR errorstatus=SD_OK;
	uint8_t tmp=0;	   
	//卡类型
	    cardinfo->CardType=(uint8_t)CardType; 
	//卡RCA值
        cardinfo->RCA=(uint16_t)RCA;							
        tmp=(uint8_t)((CSD_Tab[0]&0xFF000000)>>24);
	//CSD结构
	    cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;
	//2.0协议还没定义这部分(为保留),应该是后续协议定义的
        cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	
        cardinfo->SD_csd.Reserved1=tmp&0x03;			//2个保留位  
	    tmp=(uint8_t)((CSD_Tab[0]&0x00FF0000)>>16);			//第1个字节
	//数据读时间1
        cardinfo->SD_csd.TAAC=tmp;				   		
        tmp=(uint8_t)((CSD_Tab[0]&0x0000FF00)>>8);	  		//第2个字节
	//数据读时间2
        cardinfo->SD_csd.NSAC=tmp;		  				
        tmp=(uint8_t)(CSD_Tab[0]&0x000000FF);				//第3个字节
	//传输速度	
        cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		   
        tmp=(uint8_t)((CSD_Tab[1]&0xFF000000)>>24);			//第4个字节
	//卡指令类高四位
        cardinfo->SD_csd.CardComdClasses=tmp<<4;    	
        tmp=(uint8_t)((CSD_Tab[1]&0x00FF0000)>>16);	 		//第5个字节
	//卡指令类低四位
	    cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;
	//最大读取数据长度
        cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	
        tmp=(uint8_t)((CSD_Tab[1]&0x0000FF00)>>8);			//第6个字节
	//允许分块读
	    cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	
	//写块错位
	    cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	
	//读块错位
        cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	
        cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
        cardinfo->SD_csd.Reserved2=0; 					//保留
	//标准1.1/2.0卡/MMC卡
        if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))
        {
            //C_SIZE(12位)
            cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	
            tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 			//第7个字节	
            cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
            tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24);		//第8个字节	
            cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
            cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
            cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
            tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);		//第9个字节	
            cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
            cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
            //C_SIZE_MULT
            cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;
            tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8);	  	//第10个字节	
            cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
            //计算卡容量
            cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);
            cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
            //块大小
            cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);
            cardinfo->CardCapacity*=cardinfo->CardBlockSize;
        }
	//高容量卡
        else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	
        {
            tmp=(uint8_t)(CSD_Tab[1]&0x000000FF); 		//第7个字节	
            //C_SIZE
            cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;
            tmp=(uint8_t)((CSD_Tab[2]&0xFF000000)>>24); 	//第8个字节	
            cardinfo->SD_csd.DeviceSize|=(tmp<<8);
            tmp=(uint8_t)((CSD_Tab[2]&0x00FF0000)>>16);	//第9个字节	
            cardinfo->SD_csd.DeviceSize|=(tmp);
            tmp=(uint8_t)((CSD_Tab[2]&0x0000FF00)>>8); 	//第10个字节
            double tempDouble = (double)(cardinfo->SD_csd.DeviceSize+1);
            tempDouble = (tempDouble*1024.0*512.0);
            //计算卡容量
            cardinfo->CardCapacity=tempDouble;
            //块大小固定为512字节
            cardinfo->CardBlockSize=512; 			
        }	  
	cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
	cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;	   
	tmp=(uint8_t)(CSD_Tab[2]&0x000000FF);			//第11个字节	
	cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
	cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
	tmp=(uint8_t)((CSD_Tab[3]&0xFF000000)>>24);		//第12个字节	
	cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
	cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
	cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
	cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;	 
	tmp=(uint8_t)((CSD_Tab[3]&0x00FF0000)>>16);		//第13个字节
	cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
	cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
	cardinfo->SD_csd.Reserved3=0;
	cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);  
	tmp=(uint8_t)((CSD_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
	cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
	cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
	cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
	cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
	cardinfo->SD_csd.ECC=(tmp&0x03);  
	tmp=(uint8_t)(CSD_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_csd.CSD_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_csd.Reserved4=1;		 
	tmp=(uint8_t)((CID_Tab[0]&0xFF000000)>>24);		//第0个字节
	cardinfo->SD_cid.ManufacturerID=tmp;		    
	tmp=(uint8_t)((CID_Tab[0]&0x00FF0000)>>16);		//第1个字节
	cardinfo->SD_cid.OEM_AppliID=tmp<<8;	  
	tmp=(uint8_t)((CID_Tab[0]&0x000000FF00)>>8);		//第2个字节
	cardinfo->SD_cid.OEM_AppliID|=tmp;	    
	tmp=(uint8_t)(CID_Tab[0]&0x000000FF);			//第3个字节	
	cardinfo->SD_cid.ProdName1=tmp<<24;				  
	tmp=(uint8_t)((CID_Tab[1]&0xFF000000)>>24); 		//第4个字节
	cardinfo->SD_cid.ProdName1|=tmp<<16;	  
	tmp=(uint8_t)((CID_Tab[1]&0x00FF0000)>>16);	   	//第5个字节
	cardinfo->SD_cid.ProdName1|=tmp<<8;		 
	tmp=(uint8_t)((CID_Tab[1]&0x0000FF00)>>8);		//第6个字节
	cardinfo->SD_cid.ProdName1|=tmp;		   
	tmp=(uint8_t)(CID_Tab[1]&0x000000FF);	  		//第7个字节
	cardinfo->SD_cid.ProdName2=tmp;			  
	tmp=(uint8_t)((CID_Tab[2]&0xFF000000)>>24); 		//第8个字节
	cardinfo->SD_cid.ProdRev=tmp;		 
	tmp=(uint8_t)((CID_Tab[2]&0x00FF0000)>>16);		//第9个字节
	cardinfo->SD_cid.ProdSN=tmp<<24;	   
	tmp=(uint8_t)((CID_Tab[2]&0x0000FF00)>>8); 		//第10个字节
	cardinfo->SD_cid.ProdSN|=tmp<<16;	   
	tmp=(uint8_t)(CID_Tab[2]&0x000000FF);   			//第11个字节
	cardinfo->SD_cid.ProdSN|=tmp<<8;		   
	tmp=(uint8_t)((CID_Tab[3]&0xFF000000)>>24); 		//第12个字节
	cardinfo->SD_cid.ProdSN|=tmp;			     
	tmp=(uint8_t)((CID_Tab[3]&0x00FF0000)>>16);	 	//第13个字节
	cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
	cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;    
	tmp=(uint8_t)((CID_Tab[3]&0x0000FF00)>>8);		//第14个字节
	cardinfo->SD_cid.ManufactDate|=tmp;		 	  
	tmp=(uint8_t)(CID_Tab[3]&0x000000FF);			//第15个字节
	cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
	cardinfo->SD_cid.Reserved2=1;	 
	return errorstatus;
}

//选卡
//发送CMD7,选择相对地址(rca)为addr的卡,取消其他卡.如果为0,则都不选择.
//addr:卡的RCA地址
SD_ERROR BoardSD_SelectDeselect(uint32_t addr)
{
    //发送CMD7,选择卡,短响应
    MCU_SDIO_SendCommand(addr,SD_CMD_SEL_DESEL_CARD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
	return MCU_SDIO_CheckCmdR1Resp(SD_CMD_SEL_DESEL_CARD);	  
}


//查找SD卡的SCR寄存器值 rca:卡相对地址 pscr:数据缓存区(存储SCR内容)	   
static SD_ERROR BoardSD_FindSCR(uint16_t rca,uint32_t *pscr)
{ 
	uint32_t index = 0; 
	SD_ERROR errorstatus = SD_OK;
	uint32_t tempscr[2]={0,0};  
	//发送CMD16,短响应,设置Block Size为8字节
        MCU_SDIO_SendCommand((uint32_t)8,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCKLEN);
        if(errorstatus!=SD_OK)return errorstatus;	 
	//发送CMD55,短响应 
        MCU_SDIO_SendCommand((uint32_t) RCA << 16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_CMD);
        if(errorstatus!=SD_OK)return errorstatus;
	//8个字节长度,block为8字节,SD卡到SDIO.块大小8byte 
        MCU_SDIO_SetDataConfig(SDIO_DataBlockSize_8b,8,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToSDIO,SDIO_TransferMode_Block);		
	//发送ACMD51,短响应,参数为0	
        MCU_SDIO_SendCommand(0x0,SD_CMD_SD_APP_SEND_SCR,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SD_APP_SEND_SCR);
        if(errorstatus!=SD_OK)return errorstatus;	
    //循环等待数据接收						   
        while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
        { 
            //接收FIFO数据可用
            if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
            {
                //读取FIFO内容
                *(tempscr+index)=SDIO->FIFO;	
                index++;
                if(index>=2)break;
            }
        }
	//数据超时错误
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
        {		
            //清错误标志		
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
            return SD_DATA_TIMEOUT;
        }
	//数据块CRC错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
        {
            //清错误标志
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
            return SD_DATA_CRC_FAIL;		   
        }
	//接收fifo上溢错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
        {
            //清错误标志
            SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		
            return SD_RX_OVERRUN;		 
        }
	//接收起始位错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
        {
            //清错误标志
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;		 
        }  
	//清除所有标记
	    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	//把数据顺序按8位为单位倒过来.   	
        *(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
        *(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
        return errorstatus;
}

//SDIO使能宽总线模式
static SD_ERROR BoardSD_EnableWideBus(uint8_t enable)
{
	SD_ERROR errorstatus = SD_OK;
	uint32_t scr[2]={0,0};
	uint8_t arg=0X00;
	if(enable)arg=0X02;
	else arg=0X00;
	//SD卡处于LOCKED状态
        if(SDIO->RESP1&SD_CARD_LOCKED)
            return SD_LOCK_UNLOCK_FAILED;
	//得到SCR寄存器数据
        errorstatus=BoardSD_FindSCR(RCA,scr);						
        if(errorstatus!=SD_OK)return errorstatus;
	//支持宽总线
        if((scr[1]&SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)		
        {
            //发送CMD55+RCA,短响应	
                MCU_SDIO_SendCommand((uint32_t) RCA << 16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);	
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_CMD);
                if(errorstatus!=SD_OK)return errorstatus; 
            //发送ACMD6,短响应,参数:10,4位;00,1位.
                MCU_SDIO_SendCommand(arg,SD_CMD_APP_SD_SET_BUSWIDTH,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_SD_SET_BUSWIDTH);
                return errorstatus;
        }
	//不支持宽总线设置 
        else 
        {
            return SD_REQUEST_NOT_APPLICABLE;	
        }					 
}	

//设置SDIO总线宽度(MMC卡不支持4bit模式)
//wmode:位宽模式.0,1位数据宽度;1,4位数据宽度;2,8位数据宽度
//SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
//SDIO_BusWide_4b: 4-bit data transfer
//SDIO_BusWide_1b: 1-bit data transfer (默认)
//返回值:SD卡错误状态
static SD_ERROR BoardSD_EnableWideBusOperation(uint32_t WideMode)
{
	SD_ERROR errorstatus=SD_OK;
    //多媒体卡不支持
        if (SDIO_MULTIMEDIA_CARD == CardType)
        {
            errorstatus = SD_UNSUPPORTED_FEATURE;
            return(errorstatus);
        }
	//2.0 sd不支持8bits
        else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
        {
            if (SDIO_BusWide_8b == WideMode)   
            {
                errorstatus = SD_UNSUPPORTED_FEATURE;
                return(errorstatus);
            }
            else   
            {
                errorstatus=BoardSD_EnableWideBus(WideMode);
                if(SD_OK==errorstatus)
                {
                    //清除之前的位宽设置 
                    SDIO->CLKCR&=~(3<<11);	
                    //1位/4位总线宽度 				
                    SDIO->CLKCR|=WideMode;
                    //不开启硬件流控制
                    SDIO->CLKCR|=0<<14;			
                }
            }  
        }
	return errorstatus; 
}

//读取当前卡状态 pcardstatus:卡状态
static SD_ERROR BoardSD_ReadCurrentCardStateBase(uint32_t *pcardstatus)
{
	SD_ERROR errorstatus = SD_OK;
	if(pcardstatus==NULL)
	{
		errorstatus=SD_INVALID_PARAMETER;
		return errorstatus;
	}
	//发送CMD13,短响应
        MCU_SDIO_SendCommand((uint32_t) RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);	
        //查询响应状态 
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SEND_STATUS);	
        if(errorstatus!=SD_OK)return errorstatus;
	//读取响应值
        *pcardstatus=SDIO->RESP1;
        return errorstatus;
} 

//返回SD卡的状态
SDCardState BoardSD_ReadCurrentCardState(void)
{
	uint32_t resp1=0;
	if(BoardSD_ReadCurrentCardStateBase(&resp1)!=SD_OK)
		return SD_CARD_ERROR;
	else 
		return (SDCardState)((resp1>>9) & 0x0F);
}

//检查卡是否正在执行写操作
//pstatus:当前状态.
static SD_ERROR BoardSD_IsCardProgramming(uint8_t *pstatus)
{
	volatile uint32_t respR1 = 0, status = 0;  
	//发送CMD13,卡相对地址参数
        MCU_SDIO_SendCommand((uint32_t) RCA << 16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        status=SDIO->STA;
        //等待操作完成
        while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;
	//CRC检测失败
        if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)			
        {  
            //清除错误标记
            SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);	
            return SD_CMD_CRC_FAIL;
        }
	//命令超时 
        if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)			
        {
            //清除错误标记
            SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			
            return SD_CMD_RSP_TIMEOUT;
        }
    //错误指令
	    if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
	//清除所有标记
	    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    //返回状态
        respR1=SDIO->RESP1;
        *pstatus=(uint8_t)((respR1>>9)&0x0000000F);
	return SD_OK;
}

//端口初始化
void BoardSD_PortInit(void)
{
    OS_ERR err;	 
    //SDIO接口初始化
	    MCU_SDIO_PortInit();
	//创建一个互斥信号量,用于SD卡读写
        OSMutexCreate((OS_MUTEX*	)&mutexBoard_SDCARD,
                        (CPU_CHAR*	)"mutexBoard_SDCARD",
                        (OS_ERR*		)&err);
}

//初始化SD卡
//返回值:错误代码;(0,无错误)
SD_ERROR BoardSD_Init(void)
{
	SD_ERROR errorstatus=SD_OK;
    uint8_t clkdiv=0;
	//SD卡上电
	    errorstatus=BoardSD_PowerON();
	//初始化SD卡	
	    if(errorstatus==SD_OK)errorstatus=BoardSD_InitializeCards();
	//获取卡信息
	    if(errorstatus==SD_OK)errorstatus=BoardSD_GetCardInfo(&SDCardInfo);
	//选中SD卡
	    if(errorstatus==SD_OK)errorstatus=BoardSD_SelectDeselect((uint32_t)(SDCardInfo.RCA<<16)); 	
	//4位宽度,如果是MMC卡,则不能用4位模式 
	    if(errorstatus==SD_OK)errorstatus=BoardSD_EnableWideBusOperation(SDIO_BusWide_4b);	
	//根据初始化结果设置SD卡频率	
        if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
        {  		    
            if(SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
            {
                //V1.1/V2.0卡，设置最高48/4=12Mhz
                clkdiv = SDIO_TRANSFER_CLK_DIV+2;	
            }
            else 
                //SDHC等其他卡，设置最高48/2=24Mhz
                clkdiv = SDIO_TRANSFER_CLK_DIV;	
            //设置时钟频率,SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz 
            MCU_SDIO_ClockSet(clkdiv);	
        }
	return errorstatus;		 
}


//SD卡读取一个块 
//buf:读数据缓存区(必须4字节对齐!!)
//addr:读取地址
//blksize:块大小
static SD_ERROR BoardSD_ReadBlock(uint8_t *buf,long long addr,uint16_t blksize)
{	  
    //初始状态
	SD_ERROR errorstatus=SD_OK;
	uint8_t power;
	//转换为uint32_t指针
	uint32_t count=0,*tempbuff=(uint32_t*)buf; 
	uint32_t timeout=SDIO_DATATIMEOUT;   
	if(NULL==buf)
	return SD_INVALID_PARAMETER; 
	//数据控制寄存器清零(关DMA) 
	    SDIO->DCTRL=0x0;	
	//大容量卡
        if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
        {
            blksize=512;
            addr>>=9;
        }   
	//清除DPSM状态机配置
        MCU_SDIO_SetDataConfig(SDIO_DataBlockSize_1b,0,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
	//卡锁了
	    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
    //设置块大小为合理大小
        if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
        {
            power=MCU_SDIO_ConvertFromBytesToPowerOfTwo(blksize);	
            //发送CMD16+设置数据长度为blksize,短响应
            MCU_SDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            //等待R1响应 
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCKLEN);	
            //响应错误
            if(errorstatus!=SD_OK)return errorstatus;   		
        }
        else 
        {
            return SD_INVALID_PARAMETER;
        }	
	//清除DPSM状态机配置
        MCU_SDIO_SetDataConfig(power<<4,blksize,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToSDIO,SDIO_TransferMode_Block);
    //发送CMD17+从addr地址出读取数据,短响应
        MCU_SDIO_SendCommand(addr,SD_CMD_READ_SINGLE_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        //等待R1响应 
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_READ_SINGLE_BLOCK); 
        //响应错误
        if(errorstatus!=SD_OK)return errorstatus; 
	//查询模式,轮询数据
        INTX_DISABLE();
    //无上溢/CRC/超时/完成(标志)/起始位错误
        while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))
        {
            //接收区半满,表示至少存了8个字
            if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						
            {
                //循环读取数据
                for(count=0;count<8;count++)			
                {
                    *(tempbuff+count)=SDIO->FIFO;
                }
                tempbuff+=8;	
                //读数据溢出时间
                timeout=0X7FFFFF; 	
            }
            //处理超时
            else 	
            {
                if(timeout==0)
                {
                    //开启总中断
                    INTX_ENABLE();
                    return SD_DATA_TIMEOUT;
                } 
                timeout--;
            }
        } 
    //数据超时错误
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
        {				
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
            return SD_DATA_TIMEOUT;
        }
    //数据块CRC错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
            return SD_DATA_CRC_FAIL;		   
        }
    //接收fifo上溢错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		
            return SD_RX_OVERRUN;		 
        }
    //接收起始位错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;		 
        }   
    //FIFO里面,还存在可用数据
        while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	
        {
            //循环读取数据
            *tempbuff=SDIO->FIFO;	
            tempbuff++;
        }
    //开启总中断
        INTX_ENABLE();
    //清除所有标记
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        return errorstatus; 
}



//SD卡读取多个块 
//buf:读数据缓存区
//addr:读取地址
//blksize:块大小
//nblks:要读取的块数
//返回值:错误状态
uint32_t *tempbuff __attribute__((aligned(4)));
static SD_ERROR BoardSD_ReadMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
	SD_ERROR errorstatus=SD_OK;
	uint8_t power;
	uint32_t count=0;
	uint32_t timeout=SDIO_DATATIMEOUT;
	//转换为uint32_t指针	
	tempbuff=(uint32_t*)buf;
	//数据控制寄存器清零(关DMA)
	    SDIO->DCTRL=0x0;	
	//大容量卡
        if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
        {
            blksize=512;
            addr>>=9;
        }  
	//清除DPSM状态机配置
        MCU_SDIO_SetDataConfig(0,0,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
	//卡锁了
	    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
    //块大小设置为正常大小
        if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
        {
            power=MCU_SDIO_ConvertFromBytesToPowerOfTwo(blksize);	    
            //发送CMD16+设置数据长度为blksize,短响应 
            MCU_SDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            //等待R1响应
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCKLEN);	  
            //响应错误
            if(errorstatus!=SD_OK)
                return errorstatus;   		 
        }
        else 
        {
            return SD_INVALID_PARAMETER;
        }	  
	//多块读
	if(nblks>1)											  
	{				
		//判断是否超过最大接收长度 
		    if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;
		//nblks*blksize,512块大小,卡到控制器
            MCU_SDIO_SetDataConfig(power<<4,nblks*blksize,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToSDIO,SDIO_TransferMode_Block);
        //发送CMD18+从addr地址出读取数据,短响应 
            MCU_SDIO_SendCommand(addr,SD_CMD_READ_MULT_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            //等待R1响应
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_READ_MULT_BLOCK); 
            //响应错误
            if(errorstatus!=SD_OK)return errorstatus;   		 
		//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
            INTX_DISABLE();
        //无上溢/CRC/超时/完成(标志)/起始位错误
            while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))
            {
                //接收区半满,表示至少存了8个字
                if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						
                {
                    //循环读取数据
                    for(count=0;count<8;count++)			
                    {
                        *(tempbuff+count)=SDIO->FIFO;
                    }
                    tempbuff+=8;
                    //读数据溢出时间
                    timeout=0X7FFFFF; 	
                }
                //处理超时
                else 	
                {
                    if(timeout==0)
                    {
                        INTX_ENABLE();
                        return SD_DATA_TIMEOUT;
                    }
                    timeout--;
                }
            }  
        //数据超时错误
            if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
            {				
                //清错误标志
                INTX_ENABLE();
                SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
                return SD_DATA_TIMEOUT;
            }
        //数据块CRC错误
            else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
            {
                //清错误标志
                INTX_ENABLE();
                SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
                return SD_DATA_CRC_FAIL;		   
            }
        //接收fifo上溢错误
            else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	
            {
                //清错误标志
                INTX_ENABLE();
                SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		
                return SD_RX_OVERRUN;		 
            }
        //接收起始位错误
            else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
            {
                //清错误标志
                INTX_ENABLE();
                SDIO_ClearFlag(SDIO_FLAG_STBITERR);
                return SD_START_BIT_ERR;		 
            }   
        //FIFO里面,还存在可用数据
            while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	
            {
                //循环读取数据
                *tempbuff=SDIO->FIFO;	
                tempbuff++;
            }
        //接收结束
            if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		
            {
                if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
                {				
                    //发送CMD12+结束传输
                    MCU_SDIO_SendCommand(0,SD_CMD_STOP_TRANSMISSION,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                    //等待R1响应 
                    errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_STOP_TRANSMISSION);  
                    if(errorstatus!=SD_OK)
                    {
                        INTX_ENABLE();
                        return errorstatus;
                    }	 
                }
            }
        //开启总中断
            INTX_ENABLE();
        //清除所有标记
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	}
	return errorstatus;
}	

//SD卡写1个块 
//buf:数据缓存区
//addr:写地址
//blksize:块大小	  
//返回值:错误状态
static SD_ERROR BoardSD_WriteBlock(uint8_t *buf,long long addr,  uint16_t blksize)
{
	SD_ERROR errorstatus = SD_OK;
	uint8_t  power=0,cardstate=0;
	uint32_t timeout=0,bytestransferred=0;
	uint32_t cardstatus=0,count=0,restwords=0;
	//总长度(字节)
        uint32_t	tlen=blksize;						
        uint32_t*tempbuff=(uint32_t*)buf;	
	//参数错误
	    if(buf==NULL)return SD_INVALID_PARAMETER;
	//数据控制寄存器清零(关DMA)
	    SDIO->DCTRL=0x0;							
	//清除DPSM状态机配置
        MCU_SDIO_SetDataConfig(0,0,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
	//卡锁了
	    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
	//大容量卡
        if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	
        {
            blksize=512;
            addr>>=9;
        } 
    //块大小配置   
        if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
        {
            power=MCU_SDIO_ConvertFromBytesToPowerOfTwo(blksize);	
            //发送CMD16+设置数据长度为blksize,短响应
            MCU_SDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            //等待R1响应 		
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCKLEN);	
            //响应错误
            if(errorstatus!=SD_OK)
                return errorstatus;   		 
        }
        else 
        {
            return SD_INVALID_PARAMETER;
        }
    //发送CMD13,查询卡的状态,短响应 	
        MCU_SDIO_SendCommand((uint32_t)RCA<<16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        //等待R1响应  
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SEND_STATUS);		
        if(errorstatus!=SD_OK)return errorstatus;
    //获取卡状态
        cardstatus=SDIO->RESP1;													  
        timeout=SD_DATATIMEOUT;
	//检查READY_FOR_DATA位是否置位
        while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	
        {
            timeout--;  
            //发送CMD13,查询卡的状态,短响应
                MCU_SDIO_SendCommand((uint32_t)RCA<<16,SD_CMD_SEND_STATUS,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                //等待R1响应		
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SEND_STATUS);	   
                if(errorstatus!=SD_OK)return errorstatus;		
            cardstatus=SDIO->RESP1;													  
        }
    //等待卡状态超时
	    if(timeout==0)return SD_ERROR_SUB;
	//发送CMD24,写单块指令,短响应
        MCU_SDIO_SendCommand(addr,SD_CMD_WRITE_SINGLE_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
        //等待R1响应
        errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_WRITE_SINGLE_BLOCK);  
        if(errorstatus!=SD_OK)return errorstatus;   	
	//单块写,不需要发送停止传输指令
	    StopCondition=0;			
	//blksize, 控制器到卡
        MCU_SDIO_SetDataConfig(power<<4,blksize,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
	    timeout=SDIO_DATATIMEOUT;
	//关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
    INTX_DISABLE();
    //数据块发送成功/下溢/CRC/超时/起始位错误
        while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))
        {
            //发送区半空,表示至少存了8个字
            if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							
            {
                //不够32字节了
                if((tlen-bytestransferred)<SD_HALFFIFOBYTES)
                {
                    restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
                    for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
                    {
                        SDIO->FIFO=*tempbuff;
                    }
                }
                else
                {
                    for(count=0;count<8;count++)
                    {
                        SDIO->FIFO=*(tempbuff+count);
                    }
                    tempbuff+=8;
                    bytestransferred+=32;
                }
                //写数据溢出时间
                timeout=0X3FFFFFFF;	
            }
            else
            {
                if(timeout==0)
                {
                    INTX_ENABLE();
                    return SD_DATA_TIMEOUT;
                }
                timeout--;
            }
        } 
    //数据超时错误
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
        {		
            //清错误标志
            INTX_ENABLE();			
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
            return SD_DATA_TIMEOUT;
        }
    //数据块CRC错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
            return SD_DATA_CRC_FAIL;		   
        }
    //接收fifo下溢错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		
            return SD_TX_UNDERRUN;		 
        }
    //接收起始位错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
        {
            //清错误标志
            INTX_ENABLE();
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;		 
        }   
    //开启总中断
        INTX_ENABLE();
    //清除所有标记
        SDIO_ClearFlag(SDIO_STATIC_FLAGS); 
	//清除所有标记
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        errorstatus=BoardSD_IsCardProgramming(&cardstate);
        while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
        {
            errorstatus=BoardSD_IsCardProgramming(&cardstate);
        }   
	return errorstatus;
}

//SD卡写多个块 
//buf:数据缓存区
//addr:写地址
//blksize:块大小
//nblks:要写入的块数
//返回值:错误状态												   
static SD_ERROR BoardSD_WriteMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks)
{
	SD_ERROR errorstatus = SD_OK;
	uint8_t  power = 0, cardstate = 0;
	uint32_t timeout=0,bytestransferred=0;
	uint32_t count = 0, restwords = 0;
    //总长度(字节)
        uint32_t tlen=nblks*blksize;				
        uint32_t *tempbuff = (uint32_t*)buf;  
        if(buf==NULL)return SD_INVALID_PARAMETER; //参数错误  
    //数据控制寄存器清零(关DMA)   
	    SDIO->DCTRL=0x0;							
    //清除DPSM状态机配置	
        MCU_SDIO_SetDataConfig(0,0,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
    //卡锁了
	    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;
    //大容量卡
        if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)
        {
            blksize=512;
            addr>>=9;
        }    
    //块大小配置
        if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
        {
            power=MCU_SDIO_ConvertFromBytesToPowerOfTwo(blksize);	
            //发送CMD16+设置数据长度为blksize,短响应
            MCU_SDIO_SendCommand(blksize,SD_CMD_SET_BLOCKLEN,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCKLEN);	//等待R1响应  
            if(errorstatus!=SD_OK)return errorstatus;   	//响应错误	 

        }
        else
        {
            return SD_INVALID_PARAMETER;	 
        }
	//多块写入
	if(nblks>1)
	{				
        //识别块数据长度	  
		if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;   
        //设置要读取的块数量
		if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
		{
            //发送ACMD55,短响应 	
                MCU_SDIO_SendCommand((uint32_t)RCA<<16,SD_CMD_APP_CMD,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_APP_CMD);		//等待R1响应 
                if(errorstatus!=SD_OK)return errorstatus;				 
            //发送CMD23,设置块数量,短响应
                MCU_SDIO_SendCommand(nblks,SD_CMD_SET_BLOCK_COUNT,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_SET_BLOCK_COUNT);//等待R1响应 
                if(errorstatus!=SD_OK)return errorstatus;		
		} 
        //发送CMD25,多块写指令,短响应 	
            MCU_SDIO_SendCommand(addr,SD_CMD_WRITE_MULT_BLOCK,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);
            errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_WRITE_MULT_BLOCK);	//等待R1响应   		   
            if(errorstatus!=SD_OK)return errorstatus;
        //blksize, 控制器到卡	
            MCU_SDIO_SetDataConfig(power<<4,nblks*blksize,SD_DATATIMEOUT,SDIO_DPSM_Enable,SDIO_TransferDir_ToCard,SDIO_TransferMode_Block);
        //超时设定
		    timeout=SDIO_DATATIMEOUT;
        //关闭总中断(POLLING模式,严禁中断打断SDIO读写操作!!!)
            INTX_DISABLE();
        //下溢/CRC/数据结束/超时/起始位错误
            while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))
            {
                if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							//发送区半空,表示至少存了8字(32字节)
                {	  
                    if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//不够32字节了
                    {
                        restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
                        for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
                        {
                            SDIO->FIFO=*tempbuff;
                        }
                    }else 										//发送区半空,可以发送至少8字(32字节)数据
                    {
                        for(count=0;count<SD_HALFFIFO;count++)
                        {
                            SDIO->FIFO=*(tempbuff+count);
                        }
                        tempbuff+=SD_HALFFIFO;
                        bytestransferred+=SD_HALFFIFOBYTES;
                    }
                    timeout=0X3FFFFFFF;	//写数据溢出时间
                }else
                {
                    if(timeout==0)
                    {
                        INTX_ENABLE();//开启总中断
                        return SD_DATA_TIMEOUT; 
                    }
                    timeout--;
                }
            } 
        //数据超时错误
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		
        {			
            INTX_ENABLE();//开启总中断							   
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	
            return SD_DATA_TIMEOUT;
        }
        //数据块CRC错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	
        {
            INTX_ENABLE();//开启总中断
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		
            return SD_DATA_CRC_FAIL;		   
        }
        //接收fifo下溢错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	
        {
            INTX_ENABLE();//开启总中断
            SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		
            return SD_TX_UNDERRUN;		 
        }
        //接收起始位错误
        else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	
        {
            INTX_ENABLE();//开启总中断
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;		 
        }   
        //发送结束      
        if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		
        {															 
            if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
            {   
                //发送CMD12+结束传输 
                MCU_SDIO_SendCommand(0,SD_CMD_STOP_TRANSMISSION,SDIO_Response_Short,SDIO_Wait_No,SDIO_CPSM_Enable);	
                errorstatus=MCU_SDIO_CheckCmdR1Resp(SD_CMD_STOP_TRANSMISSION);//等待R1响应   
                if(errorstatus!=SD_OK)
                {
                    INTX_ENABLE();//开启总中断
                    return errorstatus;	
                }
            }
        }
        //开启总中断
        INTX_ENABLE();
        //清除所有标记
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
	}
	//清除所有标记
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        errorstatus=BoardSD_IsCardProgramming(&cardstate);
        while((errorstatus==SD_OK)&&((cardstate==SD_CARD_PROGRAMMING)||(cardstate==SD_CARD_RECEIVING)))
        {
            errorstatus=BoardSD_IsCardProgramming(&cardstate);
        }   
	return errorstatus;	   
}


//读SD卡 buf:读数据缓存区 sector:扇区地址 cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;				  				 
uint8_t BoardSD_ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t sta=SD_OK;
	long long lsector=sector;
	uint8_t n;
	lsector<<=9;
	OS_ERR err;
	//获取信号量
	if(OSRunning)OSMutexPend (&mutexBoard_SDCARD,0,OS_OPT_PEND_BLOCKING,0,&err);
	if((uint32_t)buf%4!=0)
	{
		for(n=0;n<cnt;n++)
		{
			//单个sector的读操作
			sta=BoardSD_ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
			memcpy(buf,SDIO_DATA_BUFFER,512);
			buf+=512;
		} 
	}
	else
	{
		//单个sector的读操作
		if(cnt==1)
			sta=BoardSD_ReadBlock(buf,lsector,512); 
		//多个sector 
		else 
			sta=BoardSD_ReadMultiBlocks(buf,lsector,512,cnt); 
	}
	//释放信号量
	if(OSRunning)OSMutexPost(&mutexBoard_SDCARD,OS_OPT_POST_FIFO,&err);
	return sta;
}

//写SD卡 buf:写数据缓存区 sector:扇区地址 cnt:扇区个数	
//返回值:错误状态;0,正常;其他,错误代码;	
uint8_t BoardSD_WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
	uint8_t sta=SD_OK;
	uint8_t n;
	long long lsector=sector;
	lsector<<=9;
	OS_ERR err;
	//获取信号量
	if(OSRunning)OSMutexPend (&mutexBoard_SDCARD,0,OS_OPT_PEND_BLOCKING,0,&err);
	if((uint32_t)buf%4!=0)
	{
		for(n=0;n<cnt;n++)
		{
			memcpy(SDIO_DATA_BUFFER,buf,512);
			//单个sector的写操作
			sta=BoardSD_WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);
			buf+=512;
		} 
	}
	else
	{
		//单个sector的写操作
		if(cnt==1)
			sta=BoardSD_WriteBlock(buf,lsector,512); 
		//多个sector 		
		else 
			sta=BoardSD_WriteMultiBlocks(buf,lsector,512,cnt);	 
	}
	//释放信号量
	if(OSRunning)OSMutexPost(&mutexBoard_SDCARD,OS_OPT_POST_NO_SCHED,&err);
	return sta;
}

//打印全部SD卡信息
void ConsoleSendCardMessage(void)
{
	sd_sdio_debug_printf("SDCardInfo.CardType = %d\r\n",SDCardInfo.CardType);
	sd_sdio_debug_printf("SDCardInfo.CardCapacity = %d MB\r\n",(uint32_t)(SDCardInfo.CardCapacity/(1024*1024)));
	sd_sdio_debug_printf("SDCardInfo.CardBlockSize = %d\r\n",SDCardInfo.CardBlockSize);
	sd_sdio_debug_printf("SDCardInfo.RCA = %d\r\n",SDCardInfo.RCA);
	
	sd_sdio_debug_printf("SDCardInfo.SD_cid.CID_CRC = 0x%X\r\n",SDCardInfo.SD_cid.CID_CRC);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ManufactDate = 0x%X\r\n",SDCardInfo.SD_cid.ManufactDate);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ManufacturerID = 0x%X\r\n",SDCardInfo.SD_cid.ManufacturerID);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.OEM_AppliID = 0x%X\r\n",SDCardInfo.SD_cid.OEM_AppliID);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ProdName1 = 0x%X\r\n",SDCardInfo.SD_cid.ProdName1);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ProdName2 = 0x%X\r\n",SDCardInfo.SD_cid.ProdName2);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ProdRev = 0x%X\r\n",SDCardInfo.SD_cid.ProdRev);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.ProdSN = 0x%X\r\n",SDCardInfo.SD_cid.ProdSN);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.Reserved1 = 0x%X\r\n",SDCardInfo.SD_cid.Reserved1);
	sd_sdio_debug_printf("SDCardInfo.SD_cid.Reserved2 = 0x%X\r\n",SDCardInfo.SD_cid.Reserved2);
	
	sd_sdio_debug_printf("SDCardInfo.SD_csd.CardComdClasses = %d\r\n",SDCardInfo.SD_csd.CardComdClasses);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.ContentProtectAppli = %d\r\n",SDCardInfo.SD_csd.ContentProtectAppli);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.CopyFlag = %d\r\n",SDCardInfo.SD_csd.CopyFlag);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.CSDStruct = %d\r\n",SDCardInfo.SD_csd.CSDStruct);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.CSD_CRC = %d\r\n",SDCardInfo.SD_csd.CSD_CRC);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.DeviceSize = %d\r\n",SDCardInfo.SD_csd.DeviceSize);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.DeviceSizeMul = %d\r\n",SDCardInfo.SD_csd.DeviceSizeMul);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.DSRImpl = %d\r\n",SDCardInfo.SD_csd.DSRImpl);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.ECC = %d\r\n",SDCardInfo.SD_csd.ECC);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.EraseGrMul = %d\r\n",SDCardInfo.SD_csd.EraseGrMul);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.EraseGrSize = %d\r\n",SDCardInfo.SD_csd.EraseGrSize);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.FileFormat = %d\r\n",SDCardInfo.SD_csd.FileFormat);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.FileFormatGrouop = %d\r\n",SDCardInfo.SD_csd.FileFormatGrouop);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.ManDeflECC = %d\r\n",SDCardInfo.SD_csd.ManDeflECC);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxBusClkFrec = %d\r\n",SDCardInfo.SD_csd.MaxBusClkFrec);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxRdCurrentVDDMax = %d\r\n",SDCardInfo.SD_csd.MaxRdCurrentVDDMax);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxRdCurrentVDDMin = %d\r\n",SDCardInfo.SD_csd.MaxRdCurrentVDDMin);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxWrBlockLen = %d\r\n",SDCardInfo.SD_csd.MaxWrBlockLen);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxWrCurrentVDDMax = %d\r\n",SDCardInfo.SD_csd.MaxWrCurrentVDDMax);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.MaxWrCurrentVDDMin = %d\r\n",SDCardInfo.SD_csd.MaxWrCurrentVDDMin);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.NSAC = %d\r\n",SDCardInfo.SD_csd.NSAC);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.PartBlockRead = %d\r\n",SDCardInfo.SD_csd.PartBlockRead);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.PermWrProtect = %d\r\n",SDCardInfo.SD_csd.PermWrProtect);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.RdBlockLen = %d\r\n",SDCardInfo.SD_csd.RdBlockLen);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.RdBlockMisalign = %d\r\n",SDCardInfo.SD_csd.RdBlockMisalign);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.Reserved1 = %d\r\n",SDCardInfo.SD_csd.Reserved1);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.Reserved2 = %d\r\n",SDCardInfo.SD_csd.Reserved2);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.Reserved3 = %d\r\n",SDCardInfo.SD_csd.Reserved3);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.Reserved4 = %d\r\n",SDCardInfo.SD_csd.Reserved4);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.SysSpecVersion = %d\r\n",SDCardInfo.SD_csd.SysSpecVersion);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.TAAC = %d\r\n",SDCardInfo.SD_csd.TAAC);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.TempWrProtect = %d\r\n",SDCardInfo.SD_csd.TempWrProtect);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.WrBlockMisalign = %d\r\n",SDCardInfo.SD_csd.WrBlockMisalign);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.WriteBlockPaPartial = %d\r\n",SDCardInfo.SD_csd.WriteBlockPaPartial);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.WrProtectGrEnable = %d\r\n",SDCardInfo.SD_csd.WrProtectGrEnable);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.WrProtectGrSize = %d\r\n",SDCardInfo.SD_csd.WrProtectGrSize);
	sd_sdio_debug_printf("SDCardInfo.SD_csd.WrSpeedFact = %d\r\n",SDCardInfo.SD_csd.WrSpeedFact);
}





















  

















