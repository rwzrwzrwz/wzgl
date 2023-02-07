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

	 

//��ʱ��10000000��ԼΪ1S


//�����·�װ�İ�




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
//ReadId����:��IC��ID�ţ����ţ�
//������*idout����ȡ�Ŀ��ű��浽����ָ��Ĵ洢�ռ�
//����ֵ��0���ɹ���ȡ���ţ�1��������ʧ��
unsigned char ReadId(unsigned char *idout)
{
	unsigned char status;
	unsigned char i;
	Cmd_Read_Id[5] = 0x01;//������������ʾ
	//Cmd_Read_Id[5] = 0x00;//�رշ�������ʾ
	TxCheckSum(Cmd_Read_Id,Cmd_Read_Id[1]);		//����У���
	//Uart1_Send_Data(Cmd_Read_Id,Cmd_Read_Id[1]);		 //���Ͷ�����ID����
	HAL_UART_Transmit(&huart1,Cmd_Read_Id,Cmd_Read_Id[1],0xffff); //���Ͷ�����ID����
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//��������
	
	status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//�Խ��յ�������У��
	if(status != STATUS_OK)  //�ж�У����Ƿ���ȷ
	{
		return STATUS_ERR;
	}
	status = Uart1RxBuf[4];
	if(status != STATUS_OK)	//�ж��Ƿ���ȷ�Ķ�����
	{
		return STATUS_ERR;
	}
	if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa1))//�ж��Ƿ�Ϊ�����ŷ��ص����ݰ�
	{
		for(i=0;i<6;i++)//��ȡ����ID��6�ֽ�		 
		{
			idout[i] = Uart1RxBuf[i+5];//������ĵ�5���ֽڿ�ʼΪ���ţ�����Ϊ6�ֽ�
		}
			return STATUS_OK;		 //�ɹ�����0
		
 	} 
	return STATUS_ERR;			//ʧ�ܷ���1
}


//ReadId����:��IC�����ݿ�
//������*idout����ȡ�����ݱ��浽����ָ��Ĵ洢�ռ�
//������block�����
//����ֵ��0���ɹ���ȡ��1������ȡʧ��
unsigned char ReadDataFromBlock(unsigned char *dataout,unsigned char block)
{
	unsigned char status;
	unsigned char i;
	Cmd_Read_Block[4] = block;
	Cmd_Read_Block[5] = 0x01;//������������ʾ
//	Cmd_Read_Block[5] = 0x00;//�رշ�������ʾ
	TxCheckSum(Cmd_Read_Block,Cmd_Read_Block[1]);	//����У��
	//Uart2_Send_Data(Cmd_Read_Block,Cmd_Read_Block[1]);		 //���Ͷ����ݿ�����
	HAL_UART_Transmit(&huart1,Cmd_Read_Block,Cmd_Read_Block[1],0xffff); 
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//��������
	status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//�Խ��յ�������У��
	if(status != STATUS_OK)		 //�ж�У����Ƿ���ȷ
	{
		return 	STATUS_ERR;
	}
	status = Uart1RxBuf[4];		//��ȡ���ذ�״̬
	if(status != STATUS_OK)	//�ж��Ƿ���ȷ�Ķ�����
	{
		return STATUS_ERR;
	}
	if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa3))//�ж��Ƿ�Ϊ�������ݷ��ص����ݰ�
	{
		for(i=0;i<16;i++)//��ȡ�����ݣ�16�ֽ�	��һ�����ݿ�Ĵ�СΪ16�ֽ�	 
	  {
			dataout[i] = Uart1RxBuf[i+5];//������ĵ�5���ֽڿ�ʼΪ���ݣ�����Ϊ16�ֽ�
		}
		return STATUS_OK;		 //�ɹ�����0
	}
	return STATUS_ERR;			//ʧ�ܷ���1
}
//ReadId����:д���ݵ�ָ�������ݿ�
//������*idout��ָ��Ҫд�����ݵĻ�����
//������block�����
//����ֵ��0��д��ɹ���1��д��ʧ��
unsigned char WriteDataToBlock(unsigned char *datain,unsigned char block)
{
	unsigned char status;
	unsigned char i;
	Cmd_Write_Block[4] = block;
	for(i=0;i<16;i++)
	{
		Cmd_Write_Block[6+i] = datain[i];
	}
	TxCheckSum(Cmd_Write_Block,Cmd_Write_Block[1]);	//����У��
	HAL_UART_Transmit(&huart1,Cmd_Write_Block,Cmd_Write_Block[1],0xffff); //����д����
	HAL_Delay(200);
	HAL_UART_Receive(&huart1,Uart1RxBuf,UART1_RX_BUF_LEN,0xffff);//��������
	//Uart2_Send_Data(Cmd_Write_Block,Cmd_Write_Block[1]);		 //����д����


		status = RxCheckSum(Uart1RxBuf,Uart1RxBuf[1]);//�Է��ص����ݽ���У��
		if(status != STATUS_OK) //�ж�У���Ƿ�ͨ��
		{
			return STATUS_ERR;
		}
		status = Uart1RxBuf[4];
		if(status != STATUS_OK) //�ж�У���Ƿ�ͨ��
		{
			return STATUS_ERR;
		}
		if((Uart1RxBuf[0] == 0x01)&&(Uart1RxBuf[2] == 0xa4))//�ж��Ƿ�Ϊд�����ݷ��ص����ݰ�
		{
				return STATUS_OK;		 //�ɹ�����0
		}
	return STATUS_ERR;			//ʧ�ܷ���1
}


