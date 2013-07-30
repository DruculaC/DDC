#include"N79E81x.h"
#include<intrins.h>
#include"AD.h"
#include"UART.h"
#include"T0.h"
#include"voice.h"
#include"pwm.h"
#include"T1.h"

//定义通信命令

#define CmdStart 0x00 //开机命令
#define CmdStop 0x01  //关机命令

#define ComMode_1 0xc1 //通信模式1 
#define ComMode_2 0xc2 //通信模式2
#define ComMode_3 0xc3 //通信模式3
#define ComMode_4 0xc4 //抬起指令
#define ComMode_5 0xc5//倒地指令

#define Succeed 0xce  //通信成功
#define Wrong 0xff    //通信失败

#define CmdHead 0xc8
#define CmdHead1 0x33 //数据帧的首部1, 00110011,11
#define CmdHead2 0xcc //数据帧的首部2,11001100,00
#define CmdHead3 0x3c //数据帧的首部3,11000011,01
#define CmdHead4 0xcc //数据帧的首部4,11001100,00

#define MyAddress 0xe0
#define MyAddress1 0x33 //本机地址1, 00110011,11
#define MyAddress2 0x3c //本机地址2, 11000011,01
#define MyAddress3 0xcc //本机地址3,11001100,00
#define MyAddress4 0xcc //本机地址4,11001100,00

//附机的发射部分的控制端口
//sbit PWMout=P3^5;//发射机的方波输出口，现在使用PWM外设了
sbit ModeControl_1=P2^6;//发射机模式控制,0亮为1.5M模式，1灭为3M模式

//接收机控制
//sbit SwitchControl=P1^3;	//1关闭接收机，0开启接收机

//开关按键
sbit Turn=P0^3;//开关

//模式选择按键
sbit ModeChange=P0^4;

//马达控制端
sbit Moto=P2^4;

//电池控制 	AD的1号通道为电池的电量检测端
sbit BatteryControl=P1^2;

//接收机控制
//sbit SwitchControl=P1^3;	//1有效，0关闭
//发射机pin18
//sbit tran_out=P1^0;
sbit receive_en=P1^3;		//接收机使能，要加上拉电阻

//无线发射机控制
sbit tran_en=P2^7;//发射机开关，1亮为开了，0灭为关了

//开机状态标记位
bit TurnFlag=0;//0位关机状态，1为开机状态

//模式选择位，0则用模式1,1则用模式2
bit ModeFlag=0;

bit receiveFlag=0;//接收到数据标志
bit commuFlag=0;//开启通信标志

bit alarmFlag2=0;//是否继续报警标志
bit alarmFlag3=0;//是否继续报警标志
bit alarmFlag4=0;	//抬起报警
bit alarmFlag5=0;	//倒地报警
unsigned int alarmCount2=0;//报警2循环次数
unsigned int alarmCount3=0;//报警3循环次数
unsigned int alarmCount4=0;//抬起报警循环次数
unsigned int alarmCount5=0;//倒地报警循环次数

bit threeFlag=0;//三路循环开关标志

bit power1Flag=0;
bit power2Flag=0;
bit power3Flag=0;
bit power4Flag=0;

unsigned char voiceFlag=0;//声音循环开关 

unsigned char dataFirst=0;//用于存储上次编码类型

unsigned char count=0;//串口接收部分的计数器
 
unsigned int time0Count_3=0;//作为串口每秒主辅机的信息交互时钟

unsigned int lastAddr=0;//上一次接收到的编码的地址
unsigned int TestFlag=0;//1、2、3分别为每1S后的计数，在串口的成功指令里会执行将去归零的操作
                		//如果连续3次都没有归零，则说明不在场了

//作为接收和发送的缓存区
unsigned char TxRxBuf[28]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//一个头字节，一个地址字节，一个命令字节，两个编码地址字节，两个编码
unsigned char myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//处理完后的通信数据的缓冲区

unsigned int time0Count_Clock=0;//电量检测时钟
unsigned int Check=0;//作为AD检测值
bit checkFlag=0;//电量检测标志
 
unsigned char DataBetween=0;//作为接收数据的中间变量
unsigned char RecData=0;//接收到的数据
unsigned char DataTime=0;//作为接收的数据的移位次数计数
bit ComFlag=1;//做上升沿的一个标志
unsigned char T1highcount=0;	   //定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将Datatime清0

//功放开关控制，1为打开功放，0为关闭功放
sbit PAshutdown=P1^4;

//定义一个计数，来表示信号接收后，多长时间使接收机打开，即控制SwitchControl的高电平时间。
//unsigned int SwitchControlcount=0;
  
//函数声明
//void codeData(unsigned char *doData,unsigned char len);		//编码 ,电平1变为0011，电平0变为1100
//void transCode(unsigned char *doData,unsigned char len);//解码，将接收到得数据还原
void ComMode_1_Data(void);//发送边码1

//开机函数
void StartAll(void);
//关机函数
void StopAll(void);

//t=1时，延迟100us左右
void Delay3(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++)		
	for(j=0;j<23;j++);
}

//init signal，发送编码信号前的起始信号，用于将接收机的自动增益打开
void initsignal()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<1;k1++)
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
//	Delay3(80);
}

void main(void)
{
//	unsigned int Check=0;//作为AD检测值	

	noVoice();
//	InitUART();
	InitT0();
	InitT1();
//	TI=0;
//	RI=0;

	Turn=1;		  //至高开关位
	ModeChange=1;

	BatteryControl=0;//电池在没有充满的情况下为低电平

	Moto=1;	//关闭马达
//	SwitchControl=0;	 //开起接收机的控制端为高电平

//	ES=1;
	ET0=1;
	ET1=1;
	PT1=1;//定时器1的中断优先级最高
	EA=1;

	myPwm();	//开发射机

//	P1M1=0x02;
//	P1M2=0x00;
	P10=1;
//	P11=1;
	PAshutdown=0;		  //开机时，将功放关闭
	
	Check=GetADCResult(6);//上电时，电池电量检测一次
	
//开机直接启动附机
//	SwitchControl=0;	 //开起接收机的控制端为高电平

	PAshutdown=1;
	SC_Speech(3);		 //车锁已打开
	Delay(100);
	PAshutdown=0;
																	   
//					StartAll();//开机给主机发送开机指令	
	commuFlag=1;//开启通信
	TurnFlag=1;

	alarmCount3=0;//清报警计数器
	alarmFlag3=0;//清报警标志

	while(1)
	{
/*
		if(Turn==0)
		{
		 	Delay(30);
			if(Turn==0)
			{
//				while(Turn==0);
				if(TurnFlag==0)		 //说明是关机状态,则开机
				{
//					SwitchControl=0;	 //开起接收机的控制端为高电平

					PAshutdown=1;
					SC_Speech(3);		 //车锁已打开
					Delay(100);
					PAshutdown=0;
																					   
//					StartAll();//开机给主机发送开机指令	
					commuFlag=1;//开启通信
					TurnFlag=1;

					alarmCount3=0;//清报警计数器
					alarmFlag3=0;//清报警标志
				}
				else
				{	 
					PAshutdown=1;
					SC_Speech(2);
					Delay(100);
					PAshutdown=0;
					Moto=1;//停止马达震动
//					StopAll();
  					commuFlag=0;//关闭通信
					TurnFlag=0;

					alarmCount3=0;//清报警计数器
					alarmFlag3=0;//清报警标志

//					Check=GetADCResult(6);//电池电量检测
				}
			}
		}
*/
		if((alarmFlag3==1)&&(alarmCount3<3))//编码3开始相应的报警
		{
			alarmCount3++;

			PAshutdown=1;
			SC_Speech(1);
			Delay(150);
			PAshutdown=0;
			
			Moto=0;//开震动
			Delay(10);
			Moto=1;
		}
//		if(alarmCount3>=2) //调节语音的段数	   
//		{
//			alarmCount3=0;//清报警计数器
//			alarmFlag3=0;//清报警标志
//		}

		if((alarmFlag4==1)&&(alarmCount4<3))//抬起开始相应的报警
		{
			alarmCount4++;

			PAshutdown=1;
			SC_Speech(5);
			Delay(180);
			
			Moto=0;//开震动
			Delay(10);
			Moto=1;
		}
//		if(alarmCount4>=1) //调节语音的段数	   
//		{
//			alarmCount4=0;//清报警计数器
//			alarmFlag4=0;//清报警标志
//		}

		if((alarmFlag5==1)&&(alarmCount5<3))//倒地开始相应的报警
		{
			alarmCount5++;

			PAshutdown=1;
			SC_Speech(4);
			Delay(180);

			Moto=0;//开震动
			Delay(10);
			Moto=1;
		}
// 		if(alarmCount5>=1) //调节语音的段数	   
//		{
//			alarmCount5=0;//清报警计数器
//			alarmFlag5=0;//清报警标志
//		}
/*
		if(Check>=0x377) //表示电池充包了
		{
			BatteryControl=1;//开漏模式，这样为高阻态	
		}
		else
		{
			BatteryControl=0;//电池在没有充满的情况下为低电平
		}
*/
/*
		if((Check>=0x36f)&&(TurnFlag==1)&&(power1Flag==0))//设置比较电压，此处为4V,以4.2V为基准
		{
			PAshutdown=1;
			SC_Speech(11);  //4V电量充足提示
			Delay(130);
			PAshutdown=0;

			power1Flag=1;
			power2Flag=0;
			power3Flag=0;
			power4Flag=0;
		}
	    else if((Check<0x36f)&&(Check>=0x365)&&(TurnFlag==1)&&(power2Flag==0))
		{
			PAshutdown=1;
			SC_Speech(10);  //3.8V电量充足提示
			Delay(130);
			PAshutdown=0;
	
			power2Flag=1;
			power1Flag=0;
			power3Flag=0;
			power4Flag=0;
		}
		else if((Check<0x365)&&(Check>=0x360)&&(TurnFlag==1)&&(power3Flag==0))
		{
			PAshutdown=1;
			SC_Speech(9);  //3.6V电量充足提示
			Delay(130);
			PAshutdown=0;

			power3Flag=1;
			power1Flag=0;
			power2Flag=0;
			power4Flag=0;
		}
		else if((Check<0x360)&&(TurnFlag==1)&&(power4Flag==0))
		{
			PAshutdown=1;
			SC_Speech(8);  //低于3.6v电量充足提示
			Delay(130);
			PAshutdown=0;

			power4Flag=1;
			power1Flag=0;
			power2Flag=0;
			power3Flag=0;
		}
*/
	}
}

void timeT1() interrupt 3 //定时器1中断接收数据
{
//	unsigned int newAddr=0;
	TH1=timer1H;//重装载
	TL1=timer1L;

	if(P11==0)//正常情况为高电平,有低电平说明有信号
	{
		DataBetween++;
		ComFlag=0;
		if(DataBetween==150)//低电平持续的最大时间	
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
//				DataTime++;
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
		myTxRxData[count]=RecData;
		if(count==0&&myTxRxData[0]==CmdHead)
		{
			count=1;
		}
		else if(count==1&&myTxRxData[1]==MyAddress)
		{
			count=2;
		}
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
	}

	if(receiveFlag==1)	//说明接收到了数据，开始处理
	{
		receiveFlag=0;	//清接收标志
		receive_en=0;			//关闭接收机
		switch(myTxRxData[2])//解析指令
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

				Moto=0;//开震动
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

	if(time0Count_3>=60)//串口每1S发送一次的数据的时间标志
	{
		Check=GetADCResult(6);//电池电量检测

		if(commuFlag==1)//说明开启了通信
		{
			receive_en=0;		//打开接收机
			ComMode_1_Data();//发送模式1信号
			receive_en=1;		//打开接收机
			TestFlag++;
			
			if(TestFlag>=4)//说明已经出了300M了。收不到任何信号了，要做报警
			{
				TestFlag=5;
				//加入相应处理代码	
			}
		}
		time0Count_3=0;
	}
}

void StartAll()	//发送开始信号
{
	unsigned char i,n;

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=CmdStart;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
	
	initsignal();

	for(i=0;i<7;i++)
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
}

void StopAll() //发送停止信号
{
	unsigned char i,n;

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=CmdStop;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;

	initsignal();

	for(i=0;i<7;i++)
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
}

void ComMode_1_Data()//发送边码1
{
	unsigned char i,n;

	ModeControl_1=0;//30M发射功率				
	tran_en=1;	//打开无线发射机
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;

	initsignal();

	for(i=0;i<7;i++)
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
	tran_en=0;
}