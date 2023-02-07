#include "rfid.h"

#define STATUS_OK			0x00
#define STATUS_ERR    0x01

#define UART1_RX_BUF_LEN  30


unsigned char Uart1RxBuf[UART1_RX_BUF_LEN];
unsigned char Uart1RxDataConut = 0;
unsigned char Rx1Flag = 0;


unsigned char Cmd_Read_Id[8] = {0x01,0x08,0xa1,0x20,0x00,0x00,0x00,0x00};
unsigned char Cmd_Read_Block[8]	= {0x01,0x08,0xa3,0x20,0x00,0x00,0x00,0x00};
unsigned char Cmd_Write_Block[23] = {0x01,0x17,0xa4,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char WBlockData[16] = {0x11,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
//CircularBuffer *Uart2_Circular_Buffer;

	 

//延时，10000000大约为1S


//发送新封装的包




unsigned char RxCheckSum(unsigned char *ptr,unsigned char len)
{
	unsigned char i;
	unsigned char checksum;
	checksum = 0;
	for(i=0;i<(len-1);i++)
	{
		   checksum ^= ptr[i];
	}
	checksum = ~checksum;
	if(ptr[len-1] == checksum)
		return 	STATUS_OK;
	else 
		return 	STATUS_ERR;
}

void TxCheckSum(unsigned char *ptr,unsigned char len)
{
	unsigned char i;
	unsigned char checksum;
	checksum = 0;
	for(i=0;i<(len-1);i++)
	{
		   checksum ^= ptr[i];
	}
	checksum = ~checksum;
	ptr[len-1] = checksum;
}
//ReadId（）:读IC卡ID号（卡号）
//参数：*idout，读取的卡号保存到它所指向的存储空间
//返回值：0：成功读取卡号，1：读卡号失败
unsigned char ReadId(unsigned char *idout)
{
	unsigned char status;
	unsigned char i;
	Cmd_Read_Id[5] = 0x01;//开启蜂鸣器提示
	//Cmd_Read_Id[5] = 0x00;//关闭蜂鸣器提示
	TxCheckSum(Cmd_Read_Id,Cmd_Read_Id[1]);		//计算校验和
	//Uart1_Send_Data(Cmd_Read_Id,Cmd_Read_Id[1]);		 //发送读卡号ID命令
	HAL_UART_Transmit(&huart1,Cmd_Read_Id,Cmd_Read_Id[1],0xffff); //发送读卡号ID命令
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//接收数据
	
	status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//对接收到的数据校验
	if(status != STATUS_OK)  //判断校验和是否正确
	{
		return STATUS_ERR;
	}
	status = Uart1RxBuf[4];
	if(status != STATUS_OK)	//判断是否正确的读到卡
	{
		return STATUS_ERR;
	}
	if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa1))//判断是否为读卡号返回的数据包
	{
		for(i=0;i<6;i++)//获取卡号ID，6字节		 
		{
			idout[i] = Uart1RxBuf[i+5];//从数组的第5个字节开始为卡号，长度为6字节
		}
			return STATUS_OK;		 //成功返回0
		
 	} 
	return STATUS_ERR;			//失败返回1
}


//ReadId（）:读IC卡数据块
//参数：*idout，读取的数据保存到它所指向的存储空间
//参数：block，块号
//返回值：0：成功读取，1：读读取失败
unsigned char ReadDataFromBlock(unsigned char *dataout,unsigned char block)
{
	unsigned char status;
	unsigned char i;
	Cmd_Read_Block[4] = block;
	Cmd_Read_Block[5] = 0x01;//开启蜂鸣器提示
//	Cmd_Read_Block[5] = 0x00;//关闭蜂鸣器提示
	TxCheckSum(Cmd_Read_Block,Cmd_Read_Block[1]);	//数据校验
	//Uart2_Send_Data(Cmd_Read_Block,Cmd_Read_Block[1]);		 //发送读数据块命令
	HAL_UART_Transmit(&huart1,Cmd_Read_Block,Cmd_Read_Block[1],0xffff); 
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//接收数据
	status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//对接收到的数据校验
	if(status != STATUS_OK)		 //判断校验和是否正确
	{
		return 	STATUS_ERR;
	}
	status = Uart1RxBuf[4];		//获取返回包状态
	if(status != STATUS_OK)	//判断是否正确的读到卡
	{
		return STATUS_ERR;
	}
	if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa3))//判断是否为读块数据返回的数据包
	{
		for(i=0;i<16;i++)//获取块数据，16字节	，一个数据块的大小为16字节	 
	  {
			dataout[i] = Uart1RxBuf[i+5];//从数组的第5个字节开始为数据，长度为16字节
		}
		return STATUS_OK;		 //成功返回0
	}
	return STATUS_ERR;			//失败返回1
}
//ReadId（）:写数据到指定的数据块
//参数：*idout，指向要写入数据的缓冲区
//参数：block，块号
//返回值：0：写入成功，1：写入失败
unsigned char WriteDataToBlock(unsigned char *datain,unsigned char block)
{
	unsigned char status;
	unsigned char i;
	Cmd_Write_Block[4] = block;
	for(i=0;i<16;i++)
	{
		Cmd_Write_Block[6+i] = datain[i];
	}
	TxCheckSum(Cmd_Write_Block,Cmd_Write_Block[1]);	//数据校验
	HAL_UART_Transmit(&huart1,Cmd_Write_Block,Cmd_Write_Block[1],0xffff); //发送写命令
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//接收数据
	//Uart2_Send_Data(Cmd_Write_Block,Cmd_Write_Block[1]);		 //发送写命令


		status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//对返回的数据进行校验
		if(status != STATUS_OK) //判断校验是否通过
		{
			return STATUS_ERR;
		}
		status = Uart1RxBuf[4];
		if(status != STATUS_OK) //判断校验是否通过
		{
			return STATUS_ERR;
		}
		if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa4))//判断是否为写块数据返回的数据包
		{
				return STATUS_OK;		 //成功返回0
		}
	return STATUS_ERR;			//失败返回1
}


