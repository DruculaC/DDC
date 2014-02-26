/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "Other.h"
#include "voice.h"
#include "AD.h"
#include "delay.h"

// ------ Public variable declarations -----------------------------
bit fell_alarm_flag = 0;			//倒地报警标志
tByte fell_alarm_count = 0;		//倒地报警循环次数

bit raised_alarm_flag = 0;		//抬起报警标志
tByte raised_alarm_count = 0;	//抬起报警循环次数

bit stolen_alarm_flag = 0;		//被盗报警标志
tWord stolen_alarm_count = 0;	//被盗报警循环次数

bit transmit_comm1_flag = 0;		//每隔一段时间将其置1，则在main函数中发射数据，就不会影响timer0的计时，发射完后将其置0.
bit battery_check = 0;				//置1时，执行一次电量转换，执行完后，将其置0
bit Host_battery_high_flag = 0;		//置1时，执行一次语音提示，表示充电已满

extern tWord ADC_check_result;		//作为AD检测值
extern bit battery_HV_flag;			//电池电位的标记，1表示现在电池是满的，0表示还没满。


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
bit battery_stolen_EN = 0;       // 作为附机接到电池被盗信号后，报警的使能

/*------------------------------------------------------------------
	timerT0()
	定时器0每次溢出后执行的操作
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// 重装在定时器的时间设置
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	// 设置一个每3s的操作
	if(++timer0_count >= 60)		
		{
      // 每个3s做一次电量检测，并进行相关的电量提示
		ADC_check_result = GetADCResult(6);
		

		
		// 判断电池电压，如果小于3.4V且原先报过高电压时，则报电量不足。如果电压大于4.2V且与原先报过电量不足时，则提示电压过高
		if((battery_HV_flag == 1)&&(ADC_check_result <= 0x333))
			{
			// 此位置0，表示电量过低
			battery_HV_flag = 0;
			Battery_low_alarm_speech();		
			}
		else if((battery_HV_flag == 0)&&(ADC_check_result >= 0x35a))
			{
			// 此位置1，表示电量过高
			battery_HV_flag = 1;
			Battery_high_alarm_speech();
			}	

		// 将计数清0
		timer0_count = 0;
//		battery_stolen_EN	= 1;
		}

 	// 主机被盗报警
	if(stolen_alarm_flag == 1)		
		{
		// 主机被盗报警次数，每报一次加1，如果大于2次，则继续加1，到一定时间后，将flag和count清0.以此来保证附机每次报警后，在短时间内
		// 再次收到报警信号，就不会报警了。
		if(++stolen_alarm_count < 3)
			{			
			// 语音提示，马达振动
			Alarm_stolen_speech();		
			Moto_Vibration();          
			}
		else
			{
			if(stolen_alarm_count >= 1200)
				{
				stolen_alarm_count = 0;
				stolen_alarm_flag = 0;
				}
			}
		}	

	// 抬起信号报警，每次报完后清0，如果再次接到则继续报。一般来说，主机每次抬起只发4遍。
	if(raised_alarm_flag == 1)	
		{
		Alarm_raised_speech();		
		Moto_Vibration();         
		raised_alarm_flag = 0;
		}

	// 倒地信号报警，每次报完后清0，如果再次接到则继续报。一般来说，主机每次倒地只发4遍。
	if(fell_alarm_flag == 1)
		{
		Alarm_fell_speech();		  
		Moto_Vibration();         
		fell_alarm_flag=0;
		}

	// 主机电量充满提示
	if(Host_battery_high_flag == 1)
		{
		Host_battery_high_alarm_speech();		
		Host_battery_high_flag = 0;
		}
		
	if(battery_stolen_EN == 1)
		{
		battery_stolen_speech();
		battery_stolen_EN = 0;
		}
	}

/*------------------------------------------------------------------
	timerT1()
	定时器1每次溢出后执行的操作
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 			
	{
	// 重装在定时器1的设置
	TH1 = timer1_8H;				
	TL1 = timer1_8L;

	// 判断接收线是否为0，如果是0，则表示可能有信号过来。
	if(receive_wire == 0)					
		{
		// 如果接收线是0，则将此标志位置0，表示此线为0过。
		receive_wire_flag=0;
		
		// 接收到低电平的时间计数，大于8ms就重新计数
		if(++receive_LV_count==120)		
			{
			receive_LV_count=0;
			}
		}
	else
	{
		if(receive_wire_flag==0)//说明有一个低电平
		{
			receive_wire_flag=1;
//			one_receive_byte<<=1;

			if((receive_LV_count>35)&&(receive_LV_count<=80))//低电平持续的时间小于3ms，则为0
			{
				one_receive_byte<<=1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			if(receive_LV_count>80)//低电平持续的时间大于4.5ms，则为1
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
		switch(received_data_buffer[2])//解析指令
		{
			case ComMode_1://接收到的是主机发送过来的编码1的信号，说明主机在3M内，是正常的
				{	
				Moto_Vibration();
				ComMode_1_Data();
				}
			break;
			
			case ComMode_2:
				{
				battery_stolen_EN = 1;
				Moto_Vibration();          
				}
		   break;
			
			case ComMode_3:
				{
				stolen_alarm_flag = 1;
				Moto_Vibration();         

				raised_alarm_count=0;//清报警计数器
				raised_alarm_flag=0;//清报警标志
				fell_alarm_count=0;//清报警计数器
				fell_alarm_flag=0;//清报警标志
				}
			break;
		
			case ComMode_4://留作抬起信号使用
			{
				raised_alarm_flag=1;//抬起报警

				stolen_alarm_count=0;//清报警计数器
				stolen_alarm_flag=0;//清报警标志
				fell_alarm_count=0;//清报警计数器
				fell_alarm_flag=0;//清报警标志
			}
			break;

			case ComMode_5://留作倒地信号使用
			{
				fell_alarm_flag=1;	//倒地报警

				stolen_alarm_count=0;//清报警计数器
				stolen_alarm_flag=0;//清报警标志
				raised_alarm_count=0;//清报警计数器
				raised_alarm_flag=0;//清报警标志
			}
			break;

			case ComMode_6:
				{
				Host_battery_high_flag = 1;
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

/*---------------------------------------------------------------------
	sEos_Go_To_Sleep()
	将MCU进入休眠状态
----------------------------------------------------------------------*/
void sEOS_Go_To_Sleep(void)
   {
   PCON |= 0x01;    // Enter idle mode (generic 8051 version)
   }

/*---------------------------------------------------
	end of file
----------------------------------------------------*/