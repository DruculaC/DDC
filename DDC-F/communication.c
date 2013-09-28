/*---------------------------------------------------
	communication.c (v1.00)
	
	通信程序
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "communication.h"
#include "Delay.h"

tByte myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};

/*--------------------------------------------------
	ComMode_1_Data()
	
	发送编码1到主机，主机会返回一个编码1信号，如果附机
	收到了，则附机知道主机在附近了。
---------------------------------------------------*/
void ComMode_1_Data()			
	{
	tByte i,n;

	ModeControl_1=0;	//30M发射功率				
	tran_en=1;	//打开无线发射机
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;

	initsignal();

	for(i=0;i<3;i++)
		{
		for(n=0;n<8;n++)
			{
			if((myTxRxData[i]&0x80)==0x80)
				{
				P10=0;
				Delay3(120);	
				}
			else	
				{
				P10=0;
				Delay3(80);
				}
			P10=1;
			myTxRxData[i]<<=1;
			Delay3(50);
			}
		}
	tran_en=0;
	}

/*----------------------------------------------------
	initsignal()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal()				
	{
	tByte k,k1;
	tByte mystartbuffer=0xaa;
	for(k1=0;k1<1;k1++)
		{
		for(k=0;k<8;k++)
			{
			if((mystartbuffer&0x80)==0x80)
				{
				P10=0;
				Delay3(80);	
				}
			else
				{
				P10=0;
				Delay3(80);	
				}
			P10=1;
			mystartbuffer<<=1;
			Delay3(150);	
			}
		mystartbuffer=0xaa;
		Delay3(80);
		}
	P10=1;
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/

