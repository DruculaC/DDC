/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-F program, for electrocar.
----------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "AD.h"
#include "T0.h"
#include "voice.h"
#include "pwm.h"
#include "T1.h"
#include "Delay.h"
#include "communication.h"

/*--------------------------------------------------*/

bit ModeFlag=0;				//模式选择位，0则用模式1,1则用模式2
bit receiveFlag=0;			//接收到数据标志
bit commuFlag=0;			//开启通信标志

bit alarmFlag2=0;			//是否继续报警标志
bit alarmFlag3=0;			//是否继续报警标志
bit alarmFlag4=0;			//抬起报警
bit alarmFlag5=0;			//倒地报警
unsigned int alarmCount2=0;	//报警2循环次数
unsigned int alarmCount3=0;	//报警3循环次数
unsigned int alarmCount4=0;	//抬起报警循环次数
unsigned int alarmCount5=0;	//倒地报警循环次数

unsigned char voiceFlag=0;	//声音循环开关 

unsigned char dataFirst=0;	//用于存储上次编码类型

unsigned char count=0;		//串口接收部分的计数器
 
unsigned int time0Count_3=0;//作为串口每秒主辅机的信息交互时钟

unsigned int lastAddr=0;	//上一次接收到的编码的地址
unsigned int TestFlag=0;	//1、2、3分别为每1S后的计数，在串口的成功指令里会执行将去归零的操作
                			//如果连续3次都没有归零，则说明不在场了


unsigned int Check=0;		//作为AD检测值
bit checkFlag=0;			//电量检测标志
 
unsigned char DataBetween=0;	//作为接收数据的中间变量
unsigned char RecData=0;		//接收到的数据
unsigned char DataTime=0;		//作为接收的数据的移位次数计数
bit ComFlag=1;					//做上升沿的一个标志
unsigned char T1highcount=0;	//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将Datatime清0

bit ADCcheck=0;			//置1时，执行一次电量转换，执行完后，将其置0
bit sendcomm1=0;		//置1时，执行一次发送编码1，执行完后，将其置0
bit sendspeech1=0;		//置1时，执行一次语音提示，表示充电已满
unsigned char speech1_count=0;	
unsigned char powerflag=1;		//电池电位的标记，1表示现在电池是满的，0表示还没满。


tByte TxRxBuf[28]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
tByte myTxRxData2[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	//接收数据的缓存

void main(void)
{
	noVoice();
	InitT0();
	InitT1();
	Moto=1;			//关闭马达

	ET0=1;
	ET1=1;
	PT1=1;			//定时器1的中断优先级最高
	EA=1;

	myPwm();		//开发射机
	P10=1;
	PAshutdown=0;		  	//开机时，将功放关闭
	
	Check=GetADCResult(6);	//上电时，电池电量检测一次
	
 	commuFlag=1;			//开启通信
	alarmCount3=0;			//清报警计数器
	alarmFlag3=0;			//清报警标志

//	receive_en=1;
	tran_en=0;

	while(1)
	{
		if(alarmFlag3==1)		//编码3开始相应的报警
		{
			if(alarmCount3<2)
			{
				PAshutdown=1;
				SC_Speech(5);
				Delay(150);
				SC_Speech(6);
				Delay(80);
				SC_Speech(7);
				Delay(120);
				PAshutdown=0;
				
				Moto=0;				
				Delay(10);
				Moto=1;
				alarmCount3++;
			}
		}

		if(alarmFlag4==1)		//抬起开始相应的报警
		{

				PAshutdown=1;
				SC_Speech(4);
				Delay(180);
				
				Moto=0;
				Delay(10);
				Moto=1;
				alarmFlag4=0;
		}

		if(alarmFlag5==1)//倒地开始相应的报警
		{
				PAshutdown=1;
				SC_Speech(8);
				Delay(180);
	
				Moto=0;//开震动
				Delay(10);
				Moto=1;

				alarmFlag5=0;
		}

		if(ADCcheck==1)				//每个3s检测一次电量，如果电池满的就检测是否低了，如果是不满的就检测是否充满。
		{
			Check=GetADCResult(6);	//电池电量检测
			ADCcheck=0;
			
			if((powerflag==1)&&(Check<=0x35a))
			{
				powerflag=0;
				PAshutdown=1;
				SC_Speech(9);	//电压不充足提示
				Delay(120);
				PAshutdown=0;
			}
			else if((powerflag==0)&&(Check>=0x377))
			{
				powerflag=1;
				PAshutdown=1;
				SC_Speech(2);	//电压充足提示
				Delay(120);
				PAshutdown=0;
			}	
		}

		if(sendcomm1==1)
		{
			receive_en=0;		//打开接收机
			ComMode_1_Data();	//发送模式1信号
			receive_en=1;		//打开接收机
			sendcomm1=0;
		}

		if(sendspeech1==1)
		{
			PAshutdown=1;
			SC_Speech(2);  
			Delay(80);
			SC_Speech(3);  
			Delay(80);
			PAshutdown=0;
			sendspeech1=0;
		}
	}
}

void timeT1() interrupt 3 				//定时器1中断接收数据
{
	TH1=timer1H;						//重装载
	TL1=timer1L;

	if(P11==0)							//正常情况为高电平,有低电平说明有信号
	{
		DataBetween++;
		ComFlag=0;
		if(DataBetween==150)			//低电平持续的最大时间	
		{
			DataBetween=0;
		}
	}
	else//为高电平了
	{
		if(ComFlag==0)//说明有一个低电平
		{
			ComFlag=1;
//			RecData<<=1;

			if((DataBetween>60)&&(DataBetween<=100))//低电平持续的时间小于3ms，则为0
			{
				RecData<<=1;
				RecData &= 0xfe;
				DataTime++;
				T1highcount=0;
			}
			if(DataBetween>100)//低电平持续的时间大于4.5ms，则为1
			{
				RecData<<=1;
				RecData |= 0x01;
				DataTime++;
				T1highcount=0;
			}
			else
			{
				T1highcount++;	
			}

			DataBetween=0;
		}
		else
		{
			T1highcount++;
			if(T1highcount>=120)
			{
				DataTime=0;
				ComFlag=1;
				count=0;
			}		
		}
	}

	if(DataTime==8)//说明一个字节的数据已经接受完全
	{
		DataTime=0;
		myTxRxData2[count]=RecData;
		if(count==0&&myTxRxData2[0]==CmdHead)
		{
			count=1;
		}
		else if(count==1&&myTxRxData2[1]==MyAddress)
		{
			count=2;
		}
		else if(count==2)
		{
			receiveFlag=1;
			count=0;
		}
		else 
		{
			count=0;
		}

/*
		else if(count>=2&&count<=5)
		{
			count++;
		}
		else if(count==6)
		{
			receiveFlag=1;
			count=0;
		}
		else 
		{
			count=0;
		}
*/
	}

	if(receiveFlag==1)	//说明接收到了数据，开始处理
	{
		receiveFlag=0;	//清接收标志
		receive_en=0;			//关闭接收机
		switch(myTxRxData2[2])//解析指令
		{
			case ComMode_1://接收到的是主机发送过来的编码1的信号，说明主机在3M内，是正常的
			{	
				TestFlag=0;//正常情况，清超时标志

				alarmCount3=0;//清报警计数器
				alarmFlag3=0;//清报警标志

				alarmCount4=0;//清报警计数器
				alarmFlag4=0;//清报警标志

				alarmCount5=0;//清报警计数器
				alarmFlag5=0;//清报警标志

				Moto=0;//开震动
				Delay(10);
				Moto=1;
			}
			break;
		
			case ComMode_3:
			{
				TestFlag=0;//清超时标志				
				alarmFlag3=1;

				alarmCount4=0;//清报警计数器
				alarmFlag4=0;//清报警标志
				alarmCount5=0;//清报警计数器
				alarmFlag5=0;//清报警标志

				Moto=0;		//开震动
				Delay(10);
				Moto=1;
			}
			break;
		
			case ComMode_4://留作抬起信号使用
			{
				TestFlag=0;//清超时标志	
				alarmFlag4=1;//抬起报警

				alarmCount3=0;//清报警计数器
				alarmFlag3=0;//清报警标志
				alarmCount5=0;//清报警计数器
				alarmFlag5=0;//清报警标志

				Moto=0;		//开震动
				Delay(10);
				Moto=1;
			}
			break;

			case ComMode_5://留作倒地信号使用
			{
				TestFlag=0;//清超时标志
				alarmFlag5=1;	//倒地报警

				alarmCount3=0;//清报警计数器
				alarmFlag3=0;//清报警标志
				alarmCount4=0;//清报警计数器
				alarmFlag4=0;//清报警标志

				Moto=0;		//开震动
				Delay(10);
				Moto=1;
			}
			break;

			case ComMode_6://留作倒地信号使用
			{
				sendspeech1=1;
			}
			break;
		}
	}
}

void time0() interrupt 1	//作为整个系统自己的时钟
{
	TH0=timer0H;//重装载
	TL0=timer0L;

	time0Count_3++;

	if(time0Count_3>=60)		//串口每1S发送一次的数据的时间标志
	{
		ADCcheck=1;
		if(commuFlag==1)		//说明开启了通信
		{
			TestFlag++;
			sendcomm1=1;
			if(TestFlag>=4)		//说明已经出了300M了。收不到任何信号了，要做报警
			{
				TestFlag=5;
			}
		}
		time0Count_3=0;
	}

	if(alarmCount3>=2)
	{
		alarmCount3++;
		if(alarmCount3==1800)
		{
			alarmCount3=0;
			alarmFlag3=0;
		}
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/