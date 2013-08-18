//电动车
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
#define ComMode_6 0xc6	//充电已满指令
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

//设置报电量的按钮，1的时候报一下电量，0的时候为常态
sbit det_battery=P2^4;			

//设置充电指示
sbit det_charge=P0^2;
//主机的发射部分的控制端口
//sbit PWMout=P0^1;//发射机的方波输出口，使用外设PWM
sbit ModeControl_1=P2^6;//发射机模式控制,0亮为30M模式，1灭为300M模式
sbit tran_en=P2^7;//发射机开关，1亮为开了，0灭为关了

//三轴传感器
sbit ReceWave=P0^7;//三轴传感器输入端
sbit SensorControl=P2^5;//三轴传感器控制端

//电磁铁,  平时值为00，开锁用10，关锁有01，然后都恢复到00
sbit MagentControl_1=P2^2;
sbit MagentControl_2=P2^3;

//拾音器控制 AD的6号通道为拾音器的音量检查端
//sbit VoiceControl=P2^4;//拾音器控制端

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
unsigned int alarmCount=0;//报警语音的次数

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

bit ADCcheck=0;			//置1时，执行一次电量转换，执行完后，将其置0
bit sendcomm1=0;		//置1时，执行一次发送编码1，执行完后，将其置0
bit sendcomm3=0;		//置1时，执行一次发送编码3，执行完后，将其置0
bit sendcomm4=0;		//置1时，执行一次发送编码4，执行完后，将其置0
bit sendcomm5=0;		//置1时，执行一次发送编码5，执行完后，将其置0
bit magcon=0;			//置1时，执行一次电磁铁锁上操作，执行完后，将其置0
bit magcon2=0;			//置1时，执行一次电磁铁打开操作，执行完后，将其置0
bit sendspeech1=0;		//置1时，执行一次报语音（被碰警告），执行完后，将其置0
bit sendspeech2=0;		//置1时，执行一次报语音（警笛音+报警），执行完后，将其置0
bit sendspeech3=0;		//置1时，执行一次报语音（被碰警告），执行完后，将其置0
bit sendspeech4=0;		//置1时，执行一次报语音（警笛音+报警），执行完后，将其置0
bit sendspeech5=0;
bit sendspeech6=0;
bit sendspeech7=0;
bit sendspeech8=0;

bit turnonflag=0;		//电动车开启关闭标志位，1表示电动车开启了，0表示电动车关闭了
unsigned char turnon_speech_flag=0;		//开机语音标志位，用来变换语音。

//函数声明
//void ComMode_1_Data(unsigned int sendAddr);	//发送模式1编码
void ComMode_1_Data(void);	//发送模式1编码
void ComMode_2_Data(void);//发送模式2编码
void ComMode_3_Data(void);//发送模式3编码
void ComMode_4_Data(void);//发送抬起编码
void ComMode_5_Data(void);//发送倒地编码
void ComMode_6_Data(void);//发送充电已满编码
void verifybattery(void);

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
//	Delay3(80);
}

void main()
{
	SensorControl=1;		  //上电关闭传感器

	noVoice();
	InitT0();
	InitT1();
	
	PAshutdown=0;		//将功放关闭

	upSignal=1;
	downSignal=1;
	ET0=1;	//开启定时器0中断
	ET1=1; //开启定时器1中断
	PT1=1;//定时器1的中断优先级最高
	EA=1;
	P10=1;
	det_charge=1;
	det_battery=1;   
	BatteryControl=0;	//附机上电的时候置0，即可以充电，电池在没有充满的情况下为低电平
	myPwm();	//开发射机

	ModeControl_1=0; //发射机模式控制端,开机时为30M模式
	
	MagentControl_1=0;//关闭磁铁
	MagentControl_2=1;
	Delay(27);
	MagentControl_1=0;//磁铁常态为这种模式
	MagentControl_2=0;
	magnetflag=0;

	commuFlag=1; //开启通信
	tran_en=0;   //关闭无线发射机

	downUpFlag=1;

	while(1)
	{
		if((det_battery==1)&&(turnonflag==0))		   //开车转动钥匙时，执行一次电量转换
		{
		 	Delay(30);
			if(det_battery==1)
			{
				verifybattery();
				
				if(turnon_speech_flag==0)
				{
					SC_Speech(7);
				}
				else
				{
					SC_Speech(8);
				}

				turnonflag=1;
			}
		}
		else if((det_battery==0)&&(turnonflag==1))
		{
		 	Delay(30);
			if(det_battery==0)
			{
				verifybattery();
				if((Check<0x300))						  //小于38V报警，小于5公里了
				{
					PAshutdown=1;
					SC_Speech(10);  //低于3.6v电量充足提示
					Delay(130);
					PAshutdown=0;
				}
				PAshutdown=1;
				SC_Speech(9);
				Delay(130);
				PAshutdown=0;
				turnonflag=0;
			}
		}
		
		if(det_charge==0)
		{
			Delay(100);
			if(det_charge==0)
			{
				PAshutdown=1;
				SC_Speech(14);
				Delay(130);
				PAshutdown=0;
			}
			
			if(Check>=0x3a3)
			{
				ComMode_6_Data(); //向附机发送编码6，表示电已充满
			}			  
		}

		if(ADCcheck==1)
		{
			Check=GetADCResult(6);	//电池电量检测
			ADCcheck=0;	
		}
		
		if(sendcomm1==1)
		{
			ComMode_1_Data(); //向附机发送编码3
			sendcomm1=0;
		}		
		
		if(sendcomm3==1)
		{
			ComMode_3_Data(); //向附机发送编码3
			sendcomm3=0;
		}

		if(sendcomm4==1)
		{
			ComMode_4_Data(); //向附机发送编码3
			sendcomm4=0;
		}

		if(sendcomm5==1)
		{
			ComMode_5_Data(); //向附机发送编码3
			sendcomm5=0;
		}

		if(magcon==1)
		{
			if(magnetflag==1)
			{
				MagentControl_1=0;//关闭磁铁
				MagentControl_2=1;
				Delay(27);
				MagentControl_1=0;//磁铁常态为这种模式
				MagentControl_2=0;
				magnetflag=0;
			}
			magcon=0;
		}

		if(magcon2==1)
		{
			if(magnetflag==0)
			{
				MagentControl_1=1;//开启磁铁
				MagentControl_2=0;
				Delay(27);
				MagentControl_1=0;//磁铁常态为这种模式
				MagentControl_2=0;
				magnetflag=1;
			}
			magcon2=0;
		}

		if(sendspeech1==1)
		{
			PAshutdown=1;
			SC_Speech(17);  //关机语言提醒
			Delay(80);
			PAshutdown=0;
			sendspeech1=0;
		}

		if(sendspeech2==1)
		{
			if(upSignal==0)
			{
				PAshutdown=1;
				SC_Speech(17);  //关机语言提醒
				Delay(100);
				SC_Speech(18);  //关机语言提醒
				Delay(100);
				SC_Speech(19);  //关机语言提醒
				Delay(100);
				PAshutdown=0;
				ComMode_3_Data(); //向附机发送编码3
			}
			else if(downSignal==0)
			{
				PAshutdown=1;
				SC_Speech(15);  //关机语言提醒
				Delay(100);
				SC_Speech(16);  //关机语言提醒
				Delay(100);
				PAshutdown=0;
				ComMode_3_Data(); //向附机发送编码3
			}
			else
			{
			 	if(alarmFlag==1)
				{
					PAshutdown=1;
					SC_Speech(22);  //关机语言提醒
					Delay(100);
					PAshutdown=0;
				}
				ComMode_3_Data(); //向附机发送编码3
				if(alarmFlag==1)
				{
					PAshutdown=1;
					SC_Speech(23);  //关机语言提醒		
					Delay(80);
					PAshutdown=0;
				}
				ComMode_3_Data(); //向附机发送编码3
			}
			

			sendspeech2=0;
		}

		if(sendspeech3==1)
		{
			PAshutdown=1;
			SC_Speech(17);  //关机语言提醒
			Delay(80);
			SC_Speech(16);  //关机语言提醒
			Delay(80);
			PAshutdown=0;
			sendspeech3=0;
		}
		if(sendspeech3==1)
		{
			PAshutdown=1;
			SC_Speech(18);  //关机语言提醒
			Delay(120);
			SC_Speech(19);  //关机语言提醒
			Delay(120);
			SC_Speech(20);  //关机语言提醒
			Delay(120);
			PAshutdown=0;
			sendspeech3=0;
		}

		if(sendspeech7==1)
		{
			PAshutdown=1;
			SC_Speech(11);  //关机语言提醒
			Delay(150);
			PAshutdown=0;
			sendspeech7=0;
		}

		if(sendspeech8==1)
		{
			PAshutdown=1;
			SC_Speech(12);  //关机语言提醒
			Delay(150);
			SC_Speech(13);
			Delay(150);
			PAshutdown=0;
			sendspeech8=0;
		}

	}
}

void timeT1() interrupt 3 //定时器1中断接收数据
{

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

	if(receiveFlag==1)
	{
		receiveFlag=0;
		switch(myTxRxData[2]) 		//对数据帧里的命令进行处理
		{
			case ComMode_1:  		//附机发送过来的只用模式1，说明现在是正常的，数据部分为数组的第一和第二个字节，为密码表内的这个编码的开始字节的那个地址，然后填充数据帧，把密码表的数据发送出去
			{
				sendcomm1=1;
				sendspeech7=1;		//编码1后报一句语音

				alarmFlag=0;		//关报警标志位
				alarmCount=0;		//报警计数次数清零
				SensorControl=0;	//三轴传感器
				downUpFlag=0; 		//关倒地、抬起检测
				downFlag=0;
				upFlag=0;
				SensorCount=0;
				time0Count_2=0;			
				magcon2=1;			//打开电磁铁

				TestFlag=0;	
				if(ModeFlag==3||ModeFlag==2)
				{
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

	if(time0Count_3>=60)//串口每3s接受一次的数据的时间标志
	{
		if(commuFlag==1)//说明开启了通信
		{
			TestFlag++;

			if(TestFlag>=3&&ModeFlag==1)//说明没有接收到数据已经有3次了，附机已经出了3M，现在就要加大功率，切换到模式2,30M再看能不能接收到数据
			{
				TestFlag=5;
				if(ModeFlag==1)
				{
					magcon=1;		 	//电磁铁锁上
					sendspeech8=1;		//报附机离开语音

					SensorControl=1;	//开启三轴传感器
					downUpFlag=1;		//开启倒地、抬起标志
					ModeFlag=2;

					SensorCount=0;
					time0Count_2=0;
				}	
 			}
 		}
		time0Count_3=0;
		
		ADCcheck=1;		
		
		if(downFlag==1)  //倒地后做相应的动作
		{
			sendcomm5=1;
		}
		if(upFlag==1)//抬起后做相应动作
		{
			sendcomm4=1;
		}
	}

	if(SensorControl==1)//检测三轴传感器是否打开
	{
		if(ReceWave==0)//说明有触发情况，开始计时
		{
			time0Count_2++;
			if(time0Count_2>=25)//说明已经大于0.5S
			{
				time0Count_2=0;//计时期清零
				SensorCount++;//三轴传感器脉冲计数加1
				alarmFlag2=1;
			}
			
			if(time0Count_1>=200)
			{
				time0Count_1=0;		//检测到低电平后，然后判断高电平是否大于200，如果是，则将其清0，如果不是，则不影响。	
			}		
 		}
		else if(ReceWave==1&&SensorCount!=0)//说明已经有一个有用的脉冲
		{
			time0Count_1++;
			if(time0Count_1>=100)//大于10S
			{
				SensorCount=0;
				time0Count_2=0;
			}
		}
	}

	if(ModeFlag==2&&SensorCount>=1)//三轴传感器脉冲的相应报警
	{
		if(SensorCount==1&&alarmFlag2==1)//三轴传感器一次触发,alarmFlag2控制发声1次
		{					
			if((upSignal!=0)&&(downSignal!=0))
			{
				sendspeech1=1;
				alarmFlag2=0;
			}
		}

		if((SensorCount==2)&&(alarmFlag2==1))
		{
			if((upSignal!=0)&&(downSignal!=0))
			{
				sendspeech3=1;
				alarmFlag2=0;
			}			
		}

		if((SensorCount==3)&&(alarmFlag2==1))
		{
			if((upSignal!=0)&&(downSignal!=0))
			{
				sendspeech4=1;
				alarmFlag2=0;
			}			
		}

		if(SensorCount>=4)//三轴传感器一次触发
		{
			ModeFlag=3;//三轴传感器已经有2次触发了，要改变发射模式了
			alarmFlag=1;//置语音报警位
			alarmFlag2=0;
			downFlag=0;
			upFlag=0;
			SensorCount=0; //脉冲计数清零
		}
	}

	if(ModeFlag==3)
	{
		sendspeech2=1;
		time0Count_4=0;			//将倒地和抬起的传感器计数置0，相当于一直屏蔽
		time0Count_5=0;

		alarmCount++;
		if(alarmCount>=1000) //调节语音的段数
		{
			alarmCount=0;//清报警计数器
			alarmFlag=0;//清报警标志
		}
	}

//	//if()//检测倒地和抬起检测的代码
	if(downUpFlag==1)//开启了抬起倒地检测
	{
		if(upSignal==0)//说明有抬起信号并且是第一次，开始计时
		{
			time0Count_4++;
			if(time0Count_4>=200)//说明已经大于0.5S
			{
				upFlag=1;//置抬起标志
				downFlag=0;
				alarmFlag=0;
			}		
		}

		if(downSignal==0)//说明有抬起信号，开始计时
		{
			time0Count_5++;
			if(time0Count_5>=200)//说明已经大于0.5S
			{
				time0Count_5 =0;//计时期清零
				downFlag=1;//置抬起标志
				upFlag=0;
				alarmFlag=0;
			}		
		}
	}
}


void ComMode_1_Data()//发送边码1
{
	unsigned char i,n;
	ModeControl_1=0;//30M发射功率				
	tran_en=1;
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
	tran_en=0;
}

void ComMode_2_Data()//发送边码2
{
//	unsigned int j;

	unsigned char i,n;
	ModeControl_1=0;//30M发射功率
	tran_en=1;
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
	tran_en=0;
}

void ComMode_3_Data()//发送边码3
{
//	unsigned int j;
	unsigned char i,n;
	ModeControl_1=1;//切换为300M发射
	tran_en=1;
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
	tran_en=0;
}

void ComMode_4_Data()//发送抬起编码
{
	unsigned char i,n;
	ModeControl_1=0;//切换为300M发射
	tran_en=1;
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
	tran_en=0;
}

void ComMode_5_Data()//发送倒地编码
{
	unsigned char i,n;
	ModeControl_1=0;//切换为300M发射
	tran_en=1;      //打开无线发射机
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
	tran_en=0;
}

void ComMode_6_Data()//发送倒地编码
{
	unsigned char i,n;
	ModeControl_1=0;//切换为300M发射
	tran_en=1;      //打开无线发射机
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_6;
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

void verifybattery()
{									  
	if((Check>=0x3a3))//设置比较电压，此处以4.96V为基准，大于47.4V（4.5V）
	{
		PAshutdown=1;
		SC_Speech(6);  	//4V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
    else if((Check<0x3a0)&&(Check>=0x37c))		//小于47.4，大于45（4.31V）
	{
		PAshutdown=1;
		SC_Speech(5);  //3.8V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check<0x378)&&(Check>=0x359))		 //小于45，大于43（4.14V）
	{
		PAshutdown=1;
		SC_Speech(4);  //3.6V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check<0x355)&&(Check>=0x327))		 //小于43，大于40（3.9V）
	{
		PAshutdown=1;
		SC_Speech(3);  //低于3.6v电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check<0x323)&&(Check>=0x304))		  //小于40，大于38（3.73V）
	{
		PAshutdown=1;
		SC_Speech(2);  //低于3.6v电量充足提示
		Delay(130);
		PAshutdown=0;
	}
}