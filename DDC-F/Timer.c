/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "Other.h"

// ------ Public variable declarations -----------------------------
bit fell_alarm_flag = 0;			//倒地报警标志
tByte fell_alarm_count = 0;		//倒地报警循环次数

bit raised_alarm_flag = 0;		//抬起报警标志
tByte raised_alarm_count = 0;	//抬起报警循环次数

bit stolen_alarm_flag = 0;		//被盗报警标志
tWord stolen_alarm_count = 0;	//被盗报警循环次数

bit transmit_comm1_flag = 0;		//每隔一段时间将其置1，则在main函数中发射数据，就不会影响timer0的计时，发射完后将其置0.
bit comm_whole_control = 0;		//通信总开关，0关闭通信，1打开通信
bit battery_check = 0;				//置1时，执行一次电量转换，执行完后，将其置0
bit Host_battery_high_flag = 0;		//置1时，执行一次语音提示，表示充电已满


// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;	//定时器0和1的定时数据

tWord timer0_count = 0;			//用来计数timer0的次数，没溢出一次就加1，相当于计时
tByte leave_count = 0;				//每隔3s加1，如果主机应答正确，则将其清零
tByte received_data_buffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	//接收数据缓存
bit receive_data_finished_flag = 0;		//接收这一串数据完成后，此标志位置1
tByte data_count = 0;				//接收数据缓存的位数，即表明第几个缓存数据
tByte one_receive_byte = 0;		//接收数据的一个字节，接收完后将其赋值给received_data_buffer相对应的字节
tByte one_receive_byte_count = 0;			//one_receive_byte有8位，此计数表明接收到第几位，每次计数到8的时候表明一个字节接收完成。
bit receive_wire_flag = 1;		//接收通信线的标志位，1表明高电平，0表明低电平，每次timer1溢出时，判断P1.1一次。以此来表明是否为一次正确的下降沿
tByte receive_HV_count = 0;		//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将one_receive_byte_count清0
tByte receive_LV_count = 0;		//每次timer1溢出时判断接收线如果为LV，则计数加1，以此来表明低电平的时间

/*------------------------------------------------------------------
	timerT0()
	定时器0每次溢出后执行的操作
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
{
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	timer0_count++;

	if(timer0_count >= 60)		//串口每1S发送一次的数据的时间标志
	{
		battery_check = 1;
		if(comm_whole_control == 1)		//说明开启了通信
		{
			leave_count++;
			transmit_comm1_flag = 1;
			if(leave_count >= 4)		//说明已经出了300M了。收不到任何信号了，要做报警
			{
				leave_count = 5;
			}
		}
		timer0_count = 0;
	}

	if(stolen_alarm_count >= 2)
	{
		stolen_alarm_count++;
		if(stolen_alarm_count == 1800)
		{
			stolen_alarm_count = 0;
			stolen_alarm_flag = 0;
		}
	}
}

/*------------------------------------------------------------------
	timerT1()
	定时器1每次溢出后执行的操作
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 				//定时器1中断接收数据
{
	TH1 = timer1_8H;					//重装载
	TL1 = timer1_8L;

	if(receive_wire == 0)							//正常情况为高电平,有低电平说明有信号
	{
		receive_LV_count++;
		receive_wire_flag=0;
		if(receive_LV_count==80)			//低电平持续的最大时间	
		{
			receive_LV_count=0;
		}
	}
	else//为高电平了
	{
		if(receive_wire_flag==0)//说明有一个低电平
		{
			receive_wire_flag=1;
//			one_receive_byte<<=1;

			if((receive_LV_count>35)&&(receive_LV_count<=50))//低电平持续的时间小于3ms，则为0
			{
				one_receive_byte<<=1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			if(receive_LV_count>50)//低电平持续的时间大于4.5ms，则为1
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

	if(receive_data_finished_flag==1)	//说明接收到了数据，开始处理
	{
		receive_data_finished_flag=0;	//清接收标志
		Receiver_EN=0;			//关闭接收机
		switch(received_data_buffer[2])//解析指令
		{
			case ComMode_1://接收到的是主机发送过来的编码1的信号，说明主机在3M内，是正常的
			{	
				leave_count=0;//正常情况，清超时标志

				stolen_alarm_count=0;//清报警计数器
				stolen_alarm_flag=0;//清报警标志

				raised_alarm_count=0;//清报警计数器
				raised_alarm_flag=0;//清报警标志

				fell_alarm_count=0;//清报警计数器
				fell_alarm_flag=0;//清报警标志

				Moto_Vibration();
			}
			break;
		
			case ComMode_3:
			{
				leave_count=0;//清超时标志				
				stolen_alarm_flag=1;

				raised_alarm_count=0;//清报警计数器
				raised_alarm_flag=0;//清报警标志
				fell_alarm_count=0;//清报警计数器
				fell_alarm_flag=0;//清报警标志

				Moto_Vibration();
			}
			break;
		
			case ComMode_4://留作抬起信号使用
			{
				leave_count=0;//清超时标志	
				raised_alarm_flag=1;//抬起报警

				stolen_alarm_count=0;//清报警计数器
				stolen_alarm_flag=0;//清报警标志
				fell_alarm_count=0;//清报警计数器
				fell_alarm_flag=0;//清报警标志

				Moto_Vibration();
			}
			break;

			case ComMode_5://留作倒地信号使用
			{
				leave_count=0;//清超时标志
				fell_alarm_flag=1;	//倒地报警

				stolen_alarm_count=0;//清报警计数器
				stolen_alarm_flag=0;//清报警标志
				raised_alarm_count=0;//清报警计数器
				raised_alarm_flag=0;//清报警标志

				Moto_Vibration();
			}
			break;

			case ComMode_6:
			{
				Host_battery_high_flag=1;
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