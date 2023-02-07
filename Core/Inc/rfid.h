#ifndef __RFID_H__
#define __RHID_H__
#include "main.h"
#include "usart.h"

unsigned char RxCheckSum(unsigned char *ptr,unsigned char len);
void TxCheckSum(unsigned char *ptr,unsigned char len);
unsigned char ReadId(unsigned char *idout);
unsigned char ReadDataFromBlock(unsigned char *dataout,unsigned char block);
unsigned char WriteDataToBlock(unsigned char *datain,unsigned char block);

#endif


