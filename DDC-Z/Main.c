/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//电动车
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "T0.h"
#include "voice.h"
#include "pwm.h"
#include "T1.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"

/*---------------------------------------------------*/

unsigned char count=0;	//数据接收部分的计数器

unsigned int lastAddr=0;//上一次接收到的编码的地址

unsigned char time0Count_1=0;//作为三轴传感器两个脉冲之间的时间间隔计时
unsigned char time0Count_2=0;//作为三轴传感器的计时
unsigned char time0Count_6=0;
unsigned int time0Count_3=0;//作为串口每秒主辅机的信息交互时钟
unsigned char time0Count_4=0;//作为抬起脉冲的时间间隔计时
unsigned char time0Count_5=0;//作为倒地脉冲的时间间隔计时

bit SensorFlag=0; //三轴传感器的低电平标志位
unsigned char  SensorCount=0; //作为三轴传感器脉冲的计数

unsigned char TestFlag=0;//1、2、3分别为每次接收到附机发送来数据后的计数，在串口的成功指令里会执行将去归零的操作
                //如果连续3次都没有归零，则说明不在场了
unsigned char ModeFlag=1;//模式选择位，1则用模式1,2则用模式2,3则为模式3

bit alarmFlag=0;//报警语音的开启标志
bit alarmFlag2=0;//报警语音标志2
//unsigned int alarmCount=0;//报警语音的次数

bit threeFlag=0;//三路循环开关标志

bit power1Flag=0;
bit power2Flag=0;
bit power3Flag=0;
bit power4Flag=0;

bit voiceFlag=0;
bit downUpFlag=0;  //倒地和抬起检测标志

bit downFlag=0;//倒地的标志
unsigned char downcount=0;
bit upFlag=0;//抬起的标志
unsigned char upcount=0;
bit downFlagSend=0;//倒地发送的标志
bit upFlagSend=0;//抬起发送的标志

//一个头字节，一个地址字节，一个命令字节，两个编码地址字节，两个编码
unsigned char myTxRxData2[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//unsigned char Check1=0;//作为AD检测值

bit receiveFlag=0;//接收到数据标志
bit commuFlag=0;//开启通信标志

unsigned char DataBetween=0;//作为接收数据的中间变量
unsigned char RecData=0;//接收到的数据
unsigned char DataTime=0;//作为接收的数据的移位次数计数
bit ComFlag=1;//做上升沿的一个标志
//unsigned int newAddr=0;
unsigned char T1highcount=0;	   //定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将Datatime清0


//定义电磁铁转动与否
unsigned char magnetflag=0;


bit ADCcheck=0;			//置1时，执行一次电量转换，执行完后，将其置0
bit sendcomm1=0;		//置1时，执行一次发送编码1，执行完后，将其置0
bit sendcomm3=0;		//置1时，执行一次发送编码3，执行完后，将其置0
bit sendcomm4=0;		//置1时，执行一次发送编码4，执行完后，将其置0
bit sendcomm5=0;		//置1时，执行一次发送编码5，执行完后，将其置0
bit magcon=0;			//置1时，执行一次电磁铁锁上操作，执行完后，将其置0
bit magcon2=0;			//置1时，执行一次电磁铁打开操作，执行完后，将其置0
bit sendspeech1=0;		//置1时，执行一次报语音（被碰警告），执行完后，将其置0
unsigned char speech1_count=0;
bit sendspeech2=0;		//置1时，执行一次报语音（警笛音+报警），执行完后，将其置0
bit sendspeech3=0;		//置1时，执行一次报语音（被碰警告），执行完后，将其置0
unsigned int speech3_count=0;
bit sendspeech4=0;		//置1时，执行一次报语音（警笛音+报警），执行完后，将其置0
bit sendspeech5=0;
bit sendspeech6=0;
bit sendspeech7=0;
unsigned char speech7_count=0;
bit sendspeech8=0;
unsigned char speech8_count=0;

bit stolenflag=0;		//被盗标志位
unsigned int stolen_count=0;	//被盗计数的时间
unsigned char stolen_flag=0;	//检测传感器开始标志

unsigned char turnonflag=0;		//电动车开启关闭标志位，1表示电动车开启了，0表示电动车关闭了
unsigned char turnon_speech_flag=0;		//开机语音标志位，用来变换语音。

unsigned int Check=0;//电量检测

void main()
{
	SensorControl=0;		  //上电关闭传感器

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
//	det_charge=1;

	BatteryControl=0;	//附机上电的时候置0，即可以充电，电池在没有充满的情况下为低电平
	myPwm();	//开发射机

	ModeControl_1=0; //发射机模式控制端,开机时为30M模式
	
	magnetflag=0;

	commuFlag=1; //开启通信
	tran_en=0;   //关闭无线发射机

	downUpFlag=1;
	det_battery=1;

	while(1)
	{
		if((det_battery==1)&&(turnonflag==0))		   //开车转动钥匙时，执行一次电量转换
		{
		 	Delay(30);
			if(det_battery==1)
			{
				verifybattery(Check);
				PAshutdown=1;
				SC_Speech(7);  
				Delay(160);
				PAshutdown=0;
				
/*				if(turnon_speech_flag==0)
				{
					PAshutdown=1;
					SC_Speech(8);  
					Delay(80);
					PAshutdown=0;
					turnon_speech_flag=1;
				}
				else
				{
					PAshutdown=1;
					SC_Speech(7);  
					Delay(240);
					PAshutdown=0;
					turnon_speech_flag=0;
				}
*/
				turnonflag=1;
			}
		}
		
		if((det_battery==0)&&(turnonflag==1))
		{
		 	Delay(30);
			if(det_battery==0)
			{
				verifybattery(Check);
				if((Check<0x300))						  //小于38V报警，小于5公里了
				{
					PAshutdown=1;
					SC_Speech(10);  //低于3.6v电量充足提示
					Delay(100);
					PAshutdown=0;
				}
				PAshutdown=1;
				SC_Speech(9);
				Delay(130);
				PAshutdown=0;
				turnonflag=0;
			}
		}

		if(ADCcheck==1)
		{
			Check=GetADCResult(6);	//电池电量检测
			ADCcheck=0;	
		}

		if(magcon==1)
		{
			if(magnetflag==1)
			{
				MagentControl_1=1;//关闭磁铁
				MagentControl_2=0;
				Delay(40);
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
				MagentControl_1=0;//开启磁铁
				MagentControl_2=1;
				Delay(40);
				MagentControl_1=0;//磁铁常态为这种模式
				MagentControl_2=0;
				magnetflag=1;
			}
			magcon2=0;
		}
/*
		if(sendspeech1==1)
		{
			PAshutdown=1;
			SC_Speech(17);  //关机语言提醒
			Delay(80);
			PAshutdown=0;
			sendspeech1=0;
		}
*/
/*
		if(sendspeech2==1)
		{
			PAshutdown=1;
			SC_Speech(18);  //关机语言提醒
			Delay(100);
			SC_Speech(16);  //关机语言提醒
			Delay(100);
			PAshutdown=0;
			sendspeech2=0;
		}
*/
		if((sendspeech3==1)&&(speech3_count<4))
		{
			if((upSignal==1)&&(downSignal==1))
			{
				PAshutdown=1;
				SC_Speech(22);  //关机语言提醒
				ComMode_3_Data();
				Delay(100);
				SC_Speech(23);  //关机语言提醒		
				ComMode_3_Data();
				Delay(60);
				PAshutdown=0;
			}
			speech3_count++;
			if(speech3_count==4)
			{
				speech3_count=0;
				sendspeech3=0;
				stolenflag=0;
			}
		}

/*		if(sendspeech3==1)
		{
			PAshutdown=1;
			SC_Speech(18);  //关机语言提醒
			Delay(120);
			SC_Speech(19);  //关机语言提醒
			Delay(140);
			SC_Speech(20);  //关机语言提醒
			Delay(170);
			PAshutdown=0;
			sendspeech3=0;
		}
*/
		if((sendspeech7==1)&&(speech7_count<1))
		{
				sendspeech8=0;
				speech8_count=0;
				
				PAshutdown=1;
				SC_Speech(11);  //关机语言提醒
				Delay(150);
				PAshutdown=0;
				speech7_count++;
		}

		if((sendspeech8==1)&&(speech8_count<1))
		{
				sendspeech7=0;
				speech7_count=0;
				
				PAshutdown=1;
				SC_Speech(12);  
				Delay(80);
				SC_Speech(13);
				Delay(80);
				PAshutdown=0;
				speech8_count++;
				SensorControl=1;	//开启三轴传感器
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

	if(receiveFlag==1)
	{
		receiveFlag=0;
		switch(myTxRxData2[2]) 		//对数据帧里的命令进行处理
		{
			case ComMode_1:  		//附机发送过来的只用模式1，说明现在是正常的，数据部分为数组的第一和第二个字节，为密码表内的这个编码的开始字节的那个地址，然后填充数据帧，把密码表的数据发送出去
			{
//				sendcomm1=1;
				stolenflag=0;
				
				ComMode_1_Data(); 	//向附机发送编码3

				sendspeech7=1;		//编码1后报一句语音

//				sendspeech8=0;
//				speech8_count=0;

				alarmFlag=0;		//关报警标志位
//				alarmCount=0;		//报警计数次数清零
				SensorControl=0;	//三轴传感器
				downUpFlag=0; 		//关倒地、抬起检测
				downFlag=0;
				upFlag=0;
				magcon2=1;			//打开电磁铁
				
				SensorCount=0;
				time0Count_2=0;
				stolen_count=0;
				stolen_flag=0;
				sendspeech1=0;
				sendspeech2=0;
				sendspeech3=0;
				speech3_count=0;

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

	if(time0Count_3>=2000)//串口每3s接受一次的数据的时间标志
	{
		if(commuFlag==1)//说明开启了通信
		{
			TestFlag++;

			if(TestFlag>=4&&ModeFlag==1)//说明没有接收到数据已经有3次了，附机已经出了3M，现在就要加大功率，切换到模式2,30M再看能不能接收到数据
			{
				TestFlag=5;
				if(ModeFlag==1)
				{
					magcon=1;		 	//电磁铁锁上
					sendspeech8=1;		//报附机离开语音
					
//					sendspeech7=0;
//					speech7_count=0;

//					SensorControl=1;	//开启三轴传感器
					downUpFlag=1;		//开启倒地、抬起标志
					ModeFlag=2;

					SensorCount=0;
					time0Count_2=0;
				}	
 			}
 		}
		time0Count_3=0;
		
//		ComMode_1_Data();

		ADCcheck=1;		
		
		if((downFlag==1)&&(downcount<5))  //倒地后做相应的动作
		{
//			sendcomm5=1;
			ComMode_5_Data(); //向附机发送编码3
			downcount++;
		}
		if((upFlag==1)&&(upcount<5))		//抬起后做相应动作
		{
//			sendcomm4=1;
			ComMode_4_Data(); //向附机发送编码3
			upcount++;
		}

/*
	if((stolenflag==1)&&(speech3_count<4))
		{
			ComMode_3_Data();	
		}
*/
	}

	if(SensorControl==1)	//检测三轴传感器是否打开，并且还没有报警
	{
		if((ReceWave==1)&&(stolenflag==0))
		{
			time0Count_2++;
			if(time0Count_2>=8)				 //每1ms检测一次高电平，如果大于了6ms的高定平，说明有人碰了一下
			{
				time0Count_2=0;
				SensorCount++;
				speech1_count=0;
				stolen_flag=1;
			}
		}
		else
		{
			time0Count_2=0;
		}
		
		if(stolen_flag==1)
		{
			stolen_count++;
			if(stolen_count>=6000)
			{
				SensorCount=0;
				time0Count_2=0;
				stolen_count=0;
				stolen_flag=0;
				sendspeech1=0;
				speech1_count=0;
				sendspeech2=0;
			}
		}
	}

	if(SensorCount==1)
	{
//		sendspeech1=1;
		if((speech1_count<1)&&(sendspeech3!=1))
		{
			if((downSignal==1)&&(upSignal==1))
			{
				PAshutdown=1;
				SC_Speech2(17);  //关机语言提醒
				Delay(80);
				PAshutdown=0;
				speech1_count++;
			}
		}
	}
	else
	{
		if(stolen_count>=3000)
		{
			if(ReceWave==1)
			{
				time0Count_6++;
				if(time0Count_6>=6)	
				{
					sendspeech3=1;
					speech3_count=0;
					stolenflag=1;
					time0Count_6=0;	
				}
			}
			else
			{
				time0Count_6=0;
			}
		}						 
	}

//	检测倒地和抬起检测的代码
	if(downUpFlag==1)//开启了抬起倒地检测
	{
		if(upSignal==0)//说明有抬起信号并且是第一次，开始计时
		{
			time0Count_4++;
			if(time0Count_4==10)//说明已经大于0.5S
			{
				upFlag=1;//置抬起标志
				downFlag=0;
				alarmFlag=0;
			}		
		}
		else
		{
			upFlag=0;
			upcount=0;
			time0Count_4=0;
		}

		if(downSignal==0)//说明有抬起信号，开始计时
		{
			time0Count_5++;
			if(time0Count_5==10)//说明已经大于0.5S
			{
				downFlag=1;//置抬起标志
				upFlag=0;
				alarmFlag=0;
			}		
		}
		else
		{
			downFlag=0;
			downcount=0;
			time0Count_5=0;
		}
	}
}


/*---------------------------------------------------
	end of file
----------------------------------------------------*/