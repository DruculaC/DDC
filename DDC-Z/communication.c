/*---------------------------------------------------
	communication.c (v1.00)
	
	通信程序
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "communication.h"
#include "Delay.h"

/*-------------------------------------------------*/
unsigned char myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};


/*----------------------------------------------------
	initsignal()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal2()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal2()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal3()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal3()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*--------------------------------------------------
	ComMode_1_Data()
	
	主机接收到编码1信号后，会反馈一个编码1信号给附机
	以表示主机在附机附近。
---------------------------------------------------*/

void ComMode_1_Data()	
{
	unsigned char i,n;
	transmiter_power=0;				
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal2();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN=0;
}

/*----------------------------------------------------------
	ComMode_3_Data()
	
	被盗报警信号，让附机语音提示“电动车被盗“
----------------------------------------------------------*/

void ComMode_3_Data()
{
//	unsigned int j;
	unsigned char i,n;
	transmiter_power=1;//切换为300M发射
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_3;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay3(50);//延时要大于2ms
		}
	}
	transmiter_EN=0;
}

/*--------------------------------------------------------
	ComMode_4_Data()
	
	发信号给附机，让其语音提示“电动车被抬起”
---------------------------------------------------------*/

void ComMode_4_Data()//发送抬起编码
{
	unsigned char i,n;
	transmiter_power=0;//切换为300M发射
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_4;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal3();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay3(50);//延时要大于2ms
		}
	}
	transmiter_EN=0;
}

/*-----------------------------------------------------------
	ComMode_5_Data()
	
	发信号给附机，让其语音提示“电动车倒地了”
------------------------------------------------------------*/

void ComMode_5_Data()//发送倒地编码
{
	unsigned char i,n;
	transmiter_power=0;//切换为300M发射
	transmiter_EN=1;      //打开无线发射机
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_5;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal3();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay3(50);//延时要大于2ms
		}
	}
	transmiter_EN=0;
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/