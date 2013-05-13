
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

//三路循环
sbit onePin=P1^6;
sbit twoPin=P1^7;
sbit threePin=P0^0;

//主机的发射部分的控制端口
//sbit PWMout=P0^1;//发射机的方波输出口，使用外设PWM
sbit ModeControl_1=P2^6;//发射机模式控制,0亮为30M模式，1灭为300M模式
//sbit ModeTurn=P2^7;//发射机开关，1亮为开了，0灭为关了

//接收机控制
//sbit SwitchControl=P1^3;//1有效，0关闭

//三轴传感器
sbit ReceWave=P0^7;//三轴传感器输入端
sbit SensorControl=P2^5;//三轴传感器控制端

//电磁铁,  平时值为00，开锁用10，关锁有01，然后都恢复到00
sbit MagentControl_1=P2^2;
sbit MagentControl_2=P2^3;

//拾音器控制 AD的6号通道为拾音器的音量检查端
sbit VoiceControl=P2^4;//拾音器控制端

sbit upSignal=P0^4;//抬起信号
sbit downSignal=P0^3;//倒地信号

//电池控制 	AD的1号通道为电池的电量检测端
sbit BatteryControl=P1^2;

unsigned char count=0;//数据接收部分的计数器

unsigned int lastAddr=0;//上一次接收到的编码的地址

unsigned char time0Count_1=0;//作为三轴传感器两个脉冲之间的时间间隔计时
unsigned char time0Count_2=0;//作为三轴传感器的计时
unsigned char time0Count_3=0;//作为串口每秒主辅机的信息交互时钟
unsigned char time0Count_4=0;//作为抬起脉冲的时间间隔计时
unsigned char time0Count_5=0;//作为倒地脉冲的时间间隔计时

bit SensorFlag=0; //三轴传感器的低电平标志位
unsigned char  SensorCount=0; //作为三轴传感器脉冲的计数

unsigned char TestFlag=0;//1、2、3分别为每次接收到附机发送来数据后的计数，在串口的成功指令里会执行将去归零的操作
                //如果连续3次都没有归零，则说明不在场了
unsigned char ModeFlag=1;//模式选择位，1则用模式1,2则用模式2,3则为模式3

bit alarmFlag=0;//报警语音的开启标志
bit alarmFlag2=0;//报警语音标志2
unsigned char alarmCount=0;//报警语音的次数

bit threeFlag=0;//三路循环开关标志

bit power1Flag=0;
bit power2Flag=0;
bit power3Flag=0;
bit power4Flag=0;

bit voiceFlag=0;
bit downUpFlag=0;  //倒地和抬起检测标志

bit downFlag=0;//倒地的标志
bit upFlag=0;//抬起的标志
bit downFlagSend=0;//倒地发送的标志
bit upFlagSend=0;//抬起发送的标志

//作为接收和发送的缓存区
unsigned char TxRxBuf[28]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//一个头字节，一个地址字节，一个命令字节，两个编码地址字节，两个编码
unsigned char myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//处理完后的通信数据的缓冲区

unsigned int Check=0;//电量检测
//unsigned char Check1=0;//作为AD检测值

bit receiveFlag=0;//接收到数据标志
bit commuFlag=0;//开启通信标志

unsigned char DataBetween=0;//作为接收数据的中间变量
unsigned char RecData=0;//接收到的数据
unsigned char DataTime=0;//作为接收的数据的移位次数计数
bit ComFlag=1;//做上升沿的一个标志
//unsigned int newAddr=0;
unsigned char T1highcount=0;	   //定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将Datatime清0

//功放开关控制，1为打开功放，0为关闭功放
sbit PAshutdown=P1^4;

//定义电磁铁转动与否
unsigned char magnetflag=0;

//定义一个计数，来表示信号接收后，多长时间使接收机打开，即控制SwitchControl的高电平时间。
//unsigned int SwitchControlcount=0;

//发送编码信号标志位
//unsigned char commode2_flag=0; //发送编码2的标志位
//unsigned char commode3_flag=0; //发送编码3的标志位

//函数声明
//void ComMode_1_Data(unsigned int sendAddr);	//发送模式1编码
void ComMode_1_Data(void);	//发送模式1编码
void ComMode_2_Data(void);//发送模式2编码
void ComMode_3_Data(void);//发送模式3编码
void ComMode_4_Data(void);//发送抬起编码
void ComMode_5_Data(void);//发送倒地编码

//void codeData(unsigned char *doData,unsigned char len);		//编码 ,电平1变为0011，电平0变为1100
//void transCode(unsigned char *doData,unsigned char len);//解码，将接收到得数据还原

//t=1时，延迟100us左右
void Delay3(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++)		
	for(j=0;j<19;j++);
}
void Delay4(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++)		
	for(j=0;j<26;j++);
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

void main()
{
	SensorControl=1;		  //上电关闭传感器

//	unsigned int newAddr=0;
	noVoice();
//	InitUART();
	InitT0();
	InitT1();
//	TI=0;
//	RI=0;

	onePin=0;
	twoPin=0;
	threePin=0;
	
	PAshutdown=0;		//将功放关闭

	upSignal=1;
	downSignal=1;
//	Delay(30);

//	ES=1;
	ET0=1;	//开启定时器0中断
	ET1=1; //开启定时器1中断
//	PS=1;
	PT1=1;//定时器1的中断优先级最高
	EA=1;
	P10=1;
//	P11=1;

	BatteryControl=0;	//附机上电的时候置0，即可以充电，电池在没有充满的情况下为低电平
	myPwm();	//开发射机

	ModeControl_1=0; //发射机模式控制端,开机时为30M模式
//	VoiceControl=0;//开机时主机开失声器
	
	MagentControl_1=0;//关闭磁铁
	MagentControl_2=1;
	Delay(27);
	MagentControl_1=0;//磁铁常态为这种模式
	MagentControl_2=0;
	magnetflag=0;

	commuFlag=1; //开启通信


//	ModeFlag=2;
	downUpFlag=1;

	while(1)
	{
/*		
		Check1=GetADCResult(5);//拾色器的检测
		if(Check1>=0x99)//设置比较电压，此处为3V设置
		{
			PAshutdown=1;	//拾音器超过某个电压时，打开功放
		}
		else
		{
			PAshutdown=0;	//拾音器一旦低于某个电压，则关闭功放
		}
*/	   	
/*
		if(Check>=0x3cf) //表示电池充包了
		{
			BatteryControl=1;//开漏模式，这样为高阻态	
		}
		else
		{
			BatteryControl=0;//电池在没有充满的情况下为低电平
		}
*/
		if((Check>=0x38a)&&(power1Flag==0))//设置比较电压，此处为4V,以4.2V为基准
		{
			PAshutdown=1;
			SC_Speech(11);  	//4V电量充足提示
			Delay(130);
			PAshutdown=0;

			power1Flag=1;
			power2Flag=0;
			power3Flag=0;
			power4Flag=0;
		}
	    else if((Check<0x389)&&(Check>=0x382)&&(power2Flag==0))
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
		else if((Check<0x37b)&&(Check>=0x377)&&(power3Flag==0))
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
		else if((Check<0x373)&&(power4Flag==0))
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

			if((DataBetween>60)&&(DataBetween<=100))	//低电平持续的时间小于10ms，则为0
			{
				RecData<<=1;
				RecData &= 0xfe;
				DataTime++;
				T1highcount=0;
			}
			else if((DataBetween>100))//低电平持续的时间大于4.5ms，则为1
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
//			DataTime++;
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
//		P01=~P01;

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

	if(receiveFlag==1)
	{
		receiveFlag=0;
		//解析命令
		switch(myTxRxData[2]) //对数据帧里的命令进行处理
		{
			case ComMode_1:  //附机发送过来的只用模式1，说明现在是正常的，数据部分为数组的第一和第二个字节，为密码表内的这个编码的开始字节的那个地址，然后填充数据帧，把密码表的数据发送出去
			{
				ComMode_1_Data();//回复确认信号

				alarmFlag=0;//关报警标志位
				alarmCount=0;//报警计数次数清零
				SensorControl=0;//三轴传感器
				downUpFlag=0; //关倒地、抬起检测
				downFlag=0;
				upFlag=0;
							
				if(magnetflag==0)
				{
					MagentControl_1=1;//开启磁铁
					MagentControl_2=0;
					Delay(27);
					MagentControl_1=0;//磁铁常态为这种模式
					MagentControl_2=0;
					magnetflag=1;
				}
 	
				TestFlag=0;	
							
				if(ModeFlag==3||ModeFlag==2)
				{
				//恢复了正常，做相应复位动作
					ModeFlag=1;
				}
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

	if(time0Count_3>=60)//串口每1S接受一次的数据的时间标志
	{
		if(commuFlag==1)//说明开启了通信
		{
			TestFlag++;

			if(TestFlag>=3&&ModeFlag==1)//说明没有接收到数据已经有3次了，附机已经出了3M，现在就要加大功率，切换到模式2,30M再看能不能接收到数据
			{
				TestFlag=5;
				if(ModeFlag==1)
				{
					MagentControl_1=0;//关闭磁铁
					MagentControl_2=1;
					Delay(27);
					MagentControl_1=0;//磁铁常态为这种模式
					MagentControl_2=0;
					magnetflag=0;

					SensorControl=1;//开启三轴传感器
					downUpFlag=1;//开启倒地、抬起标志
					ModeFlag=2;
				}	
 			}
 		}
		time0Count_3=0;
		
		Check=GetADCResult(6);//电量检测
		
		if(downFlag==1)  //倒地后做相应的动作
		{
			ComMode_5_Data();
		}
		if(upFlag==1)//抬起后做相应动作
		{
			ComMode_4_Data();
		}
	}

	if(SensorControl==1)//检测三轴传感器是否打开
	{
		if(ReceWave==1)//说明有触发情况，开始计时
		{
			time0Count_2++;
			if(time0Count_2>=10)//说明已经大于0.5S
			{
				time0Count_2 =0;//计时期清零
				SensorCount++;//三轴传感器脉冲计数加1
				alarmFlag2=1;//
			}		
 		}
		else if(ReceWave==0&&SensorCount!=0)//说明已经有一个有用的脉冲
		{
			time0Count_1++;
			if(time0Count_1>=200)//大于10S
			{
				SensorCount=0;
			}
		}
	}

	if(ModeFlag==2&&SensorCount>=1)//三轴传感器脉冲的相应报警
	{
		if(SensorCount==1&&alarmFlag2==1)//三轴传感器一次触发,alarmFlag2控制发声1次
		{					
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(5);  //关机语言提醒
			Delay(30);
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
			alarmFlag2=0;
		}
/*
		if(SensorCount==2&&alarmFlag2==1)//三轴传感器二次触发，alarmFlag2控制发声1次
		{
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(2);  //关机语言提醒
			Delay(100);
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
			alarmFlag2=0;
		}
*/
		if(SensorCount>=2)//三轴传感器一次触发
		{
//			ComMode_3_Data(lastAddr); //向附机发送编码3
  			ModeFlag=3;//三轴传感器已经有3次触发了，要改变发射模式了
			alarmFlag=1;//置语音报警位
			alarmFlag2=0;
			downFlag=0;
			upFlag=0;
			SensorCount=0; //脉冲计数清零
			Delay(1);
//			commode2_flag=0;
			ComMode_3_Data(); //向附机发送编码3
		}
	}

	if(ModeFlag==3)
	{
		if(alarmFlag==1)
		{
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(3);  //关机语言提醒
			Delay(150);
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
		}
		if(alarmFlag==1)
		{
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(4);  //关机语言提醒
			Delay(150);
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
		}
		ComMode_3_Data(); //向附机发送编码3
		if(alarmFlag==1)
		{
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(3);  //关机语言提醒
			Delay(150);
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
		}
		if(alarmFlag==1)
		{
//			VoiceControl=1;//使用语音时要关闭拾声器
			PAshutdown=1;
			SC_Speech(4);  //关机语言提醒
			Delay(150);	
			PAshutdown=0;
//			VoiceControl=0;//开启拾声器
		}
		if(alarmCount>=20) //调节语音的段数
		{
			alarmCount=0;//清报警计数器
			alarmFlag=0;//清报警标志
		}
		alarmCount++;
	}

//	#if DEBUG 1  //开启倒地和抬起检测
//	//if()//检测倒地和抬起检测的代码
	if(downUpFlag==1)//开启了抬起倒地检测
	{
		if(upSignal==0)//说明有抬起信号并且是第一次，开始计时
		{
			time0Count_4++;
			if(time0Count_4>=10)//说明已经大于0.5S
			{
				time0Count_4 =0;//计时期清零
				upFlag=1;//置抬起标志
				downFlag=0;
				alarmFlag=0;
			}		
		}

		if(downSignal==0)//说明有抬起信号，开始计时
		{
			time0Count_5++;
			if(time0Count_5>=10)//说明已经大于0.5S
			{
				time0Count_5 =0;//计时期清零
				downFlag=1;//置抬起标志
				upFlag=0;
				alarmFlag=0;
			}		
		}
	}
}

//void ComMode_1_Data(unsigned int sendAddr)//发送边码1
void ComMode_1_Data()//发送边码1
{
//	unsigned char mystartbuffer=0xbc;
//	unsigned int j;
	
	unsigned char i,n;
	ModeControl_1=0;//30M发射功率				

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
}

void ComMode_2_Data()//发送边码2
{
//	unsigned int j;

	unsigned char i,n;
	ModeControl_1=0;//30M发射功率
	
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_2;
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

void ComMode_3_Data()//发送边码3
{
//	unsigned int j;
	unsigned char i,n;
	ModeControl_1=1;//切换为300M发射

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_3;
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

void ComMode_4_Data()//发送抬起编码
{
	unsigned char i,n;
	ModeControl_1=0;//切换为300M发射

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_4;
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

void ComMode_5_Data()//发送倒地编码
{
	unsigned char i,n;
	ModeControl_1=0;//切换为300M发射

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_5;
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