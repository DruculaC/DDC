/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "voice.h"

// ------ Public variable declarations -----------------------------
bit stolen_alarm_flag = 0;		//主机被盗报警标志，1的时候表示触发		
bit host_stolen_speech_EN = 0;		//主机被盗语音使能，1的时候在main中语音提示
bit battery_check=0;		//置1时，执行一次电量转换，执行完后，将其置0
bit position_sensor_EN=0;  		//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
bit slave_away_speech_EN=0;      //判断附机离开后，语音提示，在main里面操作
bit magnet_CW_EN = 0;					//电磁铁顺时针转动使能，转动一次后复位为0
bit magnet_ACW_EN=0;						//电磁铁逆时针转动使能，转动一次后复位为0
bit comm_whole_control=0;				//通信总开关，0关闭通信，1打开通信
tWord host_stolen_speech_count=0;   //判断主机被盗后，语音提示的次数
bit slave_nearby_speech_EN=0;       //判断附近靠近后，语音提示，在main里面操作

// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;	//定时器0和1的定时数据

tByte host_touch_speech_count=0;
tByte sensor_trigger_count=0;	//传感器触发计数
tByte host_touched_flag=0;			//主机被触碰后置1，进行后面的传感器判断
tWord sensor_2ndstage_time=0;		//传感器第一阶段后流逝时间的计数
tByte sensor_1ststage_count=0;	//传感器第一阶段判断低电平的计数
tByte raised_alarm_count = 0;    //主机被抬起后，向附机发出报警信号的次数
bit raised_flag=0;					//判断主机被抬起后，置1
tByte fell_alarm_count=0;        //主机倒地后，向附机发出报警信号的次数
bit fell_flag=0;						//判断主机倒下后，置1
tWord timer0_count=0;				//timer0每次溢出后加1，表示用timer0计时
tByte slave_away=0;					//模式选择位，1则用模式1,2则用模式2,3则为模式3
tByte leave_count=0;					//附机离开时的计数，只要附机3s应答正确，就清0	
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//接收数据缓存
bit receive_data_finished_flag = 0;		//接收这一串数据完成后，此标志位置1
tByte data_count = 0;				//接收数据缓存的位数，即表明第几个缓存数据
tByte one_receive_byte = 0;		//接收数据的一个字节，接收完后将其赋值给received_data_buffer相对应的字节
tByte one_receive_byte_count = 0;			//one_receive_byte有8位，此计数表明接收到第几位，每次计数到8的时候表明一个字节接收完成。
bit receive_wire_flag = 1;		//接收通信线的标志位，1表明高电平，0表明低电平，每次timer1溢出时，判断P1.1一次。以此来表明是否为一次正确的下降沿
tByte receive_HV_count = 0;		//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将one_receive_byte_count清0
tByte receive_LV_count = 0;		//每次timer1溢出时判断接收线如果为LV，则计数加1，以此来表明低电平的时间
tByte fell_wire_time=0;          //倒地检测线，检测低电平的时间
tByte raise_wire_time=0;			//抬起检测线，检测低电平的时间
tByte sensor_2ndstage_LV_time=0; 	//传感器进入第二阶段检测时，计算低电平的时间


/*------------------------------------------------------------------
	timerT0()
	定时器0每次溢出后执行的操作
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow	//作为整个系统自己的时钟
{
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	timer0_count++;

	if(timer0_count>=2000)//串口每3s接受一次的数据的时间标志
	{
		if(comm_whole_control==1)//说明开启了通信
		{
			leave_count++;

			if(leave_count>=4&&slave_away==0)//说明没有接收到数据已经有3次了，附机已经出了3M，现在就要加大功率，切换到模式2,30M再看能不能接收到数据
			{
				leave_count=5;

				magnet_CW_EN=1;		 	//电磁铁锁上
				slave_away_speech_EN=1;		//报附机离开语音
				
				position_sensor_EN=1;		//开启倒地、抬起标志
				slave_away=1;

				sensor_trigger_count=0;
				sensor_1ststage_count=0;
					
 			}
 		}
		timer0_count=0;
		
		battery_check=1;		
		
		if((fell_flag==1)&&(fell_alarm_count<5))  //倒地后做相应的动作
		{
			ComMode_5_Data(); //向附机发送编码3
			fell_alarm_count++;
		}
		if((raised_flag==1)&&(raised_alarm_count<5))		//抬起后做相应动作
		{
//			sendcomm4=1;
			ComMode_4_Data(); //向附机发送编码3
			raised_alarm_count++;
		}
	}

	if(sensor_EN==1)	//检测三轴传感器是否打开，并且还没有报警
	{
		if((sensor_detect==0)&&(stolen_alarm_flag==0))
		{
			sensor_1ststage_count++;
			if(sensor_1ststage_count>=8)				 //每1ms检测一次高电平，如果大于了6ms的高定平，说明有人碰了一下
			{
				sensor_1ststage_count=0;
				sensor_trigger_count++;
				host_touch_speech_count=0;
				host_touched_flag=1;
			}
		}
		else
		{
			sensor_1ststage_count=0;
		}
		
		if(host_touched_flag==1)
		{
			sensor_2ndstage_time++;
			if(sensor_2ndstage_time>=6000)
			{
				sensor_trigger_count=0;
				sensor_1ststage_count=0;
				sensor_2ndstage_time=0;
				host_touched_flag=0;
				host_touch_speech_count=0;
			}
		}
	}

	if(sensor_trigger_count==1)
	{
		if((host_touch_speech_count<1)&&(host_stolen_speech_EN!=1))
		{
			if((fell_sensor_detect==1)&&(raised_sensor_detect==1))
			{
				host_touch_speech();
				host_touch_speech_count++;
			}
		}
	}
	else
	{
		if(sensor_2ndstage_time>=3000)
		{
			if(sensor_detect==0)
			{
				sensor_2ndstage_LV_time++;
				if(sensor_2ndstage_LV_time>=6)	
				{
					host_stolen_speech_EN=1;
					host_stolen_speech_count=0;
					stolen_alarm_flag=1;
					sensor_2ndstage_LV_time=0;	
				}
			}
			else
			{
				sensor_2ndstage_LV_time=0;
			}
		}						 
	}

//	检测倒地和抬起检测的代码
	if(position_sensor_EN==1)//开启了抬起倒地检测
	{
		if(raised_sensor_detect==0)//说明有抬起信号并且是第一次，开始计时
		{
			raise_wire_time++;
			if(raise_wire_time==10)//说明已经大于0.5S
			{
				raised_flag=1;//置抬起标志
				fell_flag=0;
			}		
		}
		else
		{
			raised_flag=0;
			raised_alarm_count=0;
			raise_wire_time=0;
		}

		if(fell_sensor_detect==0)
		{
			fell_wire_time++;
			if(fell_wire_time==10)//说明已经大于0.5S
			{
				fell_flag=1;//置抬起标志
				raised_flag=0;
			}		
		}
		else
		{
			fell_flag=0;
			fell_alarm_count=0;
			fell_wire_time=0;
		}
	}
}


/*------------------------------------------------------------------
	timerT1()
	定时器1每次溢出后执行的操作
--------------------------------------------------------------------*/

void timerT1() interrupt interrupt_timer_1_overflow
{

	TH1 = timer1_8H;					//重装载
	TL1 = timer1_8L;
	
	if(P11==0)//正常情况为高电平,有低电平说明有信号
	{
		receive_LV_count++;
		receive_wire_flag=0;
		if(receive_LV_count==80)//低电平持续的最大时间	
		{
			receive_LV_count=0;
		}
	}
	else//为高电平了
	{
		if(receive_wire_flag==0)//说明有一个低电平
		{
			receive_wire_flag=1;

			if((receive_LV_count>35)&&(receive_LV_count<=50))	//低电平持续的时间小于10ms，则为0
			{
				one_receive_byte<<=1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			else if((receive_LV_count>50))//低电平持续的时间大于4.5ms，则为1
			{
				one_receive_byte<<=1;
				one_receive_byte |= 0x01;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			else
			{
				receive_HV_count++;	
			}

			receive_LV_count=0;
		}

		else
		{
			receive_HV_count++;
			if(receive_HV_count>=60)
			{
				one_receive_byte_count=0;
				receive_wire_flag=1;
				data_count=0;
			}		
		}
	}

	if(one_receive_byte_count==8)//说明一个字节的数据已经接受完全
	{
		one_receive_byte_count=0;
		received_data_buffer[data_count]=one_receive_byte;
		if(data_count==0&&received_data_buffer[0]==CmdHead)
		{
			data_count=1;
		}
		else if(data_count==1&&received_data_buffer[1]==MyAddress)
		{
			data_count=2;
		}
		else if(data_count==2)
		{
			receive_data_finished_flag=1;
			data_count=0;
		}
		else 
		{
			data_count=0;
		}
	}

	if(receive_data_finished_flag==1)
	{
		receive_data_finished_flag=0;
		switch(received_data_buffer[2]) 		//对数据帧里的命令进行处理
		{
			case ComMode_1:  		//附机发送过来的只用模式1，说明现在是正常的，数据部分为数组的第一和第二个字节，为密码表内的这个编码的开始字节的那个地址，然后填充数据帧，把密码表的数据发送出去
			{
				stolen_alarm_flag=0;
				
				ComMode_1_Data(); 	//向附机发送编码3

				slave_nearby_speech_EN=1;		//编码1后报一句语音


				sensor_EN=0;	//三轴传感器
				position_sensor_EN=0; 		//关倒地、抬起检测
				fell_flag=0;
				raised_flag=0;
				magnet_ACW_EN=1;			//打开电磁铁
				
				sensor_trigger_count=0;
				sensor_1ststage_count=0;
				sensor_2ndstage_time=0;
				host_touched_flag=0;
				host_stolen_speech_EN=0;
				host_stolen_speech_count=0;

				leave_count=0;	
				if(slave_away==1)
				{
					slave_away=0;
				}
			}
			break;
		}
	}
}

/*--------------------------------------------------
	InitTimer()
	
	初始化定时器T0和T1
---------------------------------------------------*/

void InitTimer(const tByte Tick_ms_T0, Tick_us_T1)
	{
	tLong Inc_T0, Inc_T1;
	tWord timer0_16, timer1_16;
	
	//计算Timer0的寄存器值
	Inc_T0 = (tLong)Tick_ms_T0 * (OSC_FREQ/1000) / (tLong)OSC_PER_INST;
	timer0_16 = (tWord) (65536UL - Inc_T0);
	timer0_8H = (tByte) (timer0_16 / 256);
	timer0_8L = (tByte) (timer0_16 % 256);
	
	//计算Timer1的寄存器值
	Inc_T1 = (tLong)Tick_us_T1 * (OSC_FREQ/1000000) / (tLong)OSC_PER_INST;
	timer1_16 = (tWord) (65536UL - Inc_T1);
	timer1_8H = (tByte) (timer1_16 / 256);
	timer1_8L = (tByte) (timer1_16 % 256);
	
	TMOD = 0x11;
	
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	TH1 = timer1_8H;
	TL1 = timer1_8L;

	ET0 = 1;
	TR0 = 1;
	ET1 = 1;
	TR1 = 1;
	PT1 = 1;			
	EA = 1;
	}
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/