/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-F program, for electrocar.
----------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "AD.h"
#include "Timer.h"
#include "voice.h"
#include "pwm.h"
#include "Delay.h"
#include "communication.h"
#include "Other.h"

/*------- Public variable declarations --------------------------*/
extern bit fell_alarm_flag;				//倒地报警标志
extern bit raised_alarm_flag;				//抬起报警标志
extern bit stolen_alarm_flag;				//被盗报警标志
extern tWord stolen_alarm_count;     	//被盗报警循环次数
extern bit comm_whole_control;			//通信总开关，0关闭通信，1打开通信
extern bit transmit_comm1_flag;			//每隔一段时间将其置1，则在main函数中发射数据，就不会影响timer0的计时，发射完后将其置0.
extern bit battery_check;					//置1时，执行一次电量转换，执行完后，将其置0
extern bit Host_battery_high_flag;		//置1时，执行一次语音提示，表示充电已满

/*------- Private variable declarations --------------------------*/
tWord ADC_check_result = 0;		//作为AD检测值
bit battery_HV_flag = 1;			//电池电位的标记，1表示现在电池是满的，0表示还没满。

void main(void)
{
	noVoice();
	InitTimer(50, 100);
	myPwm();			//开发射机

	Moto_EN = 1;		//初始化，关闭马达
	transmit_wire = 1;
	Voice_EN = 0;		  	//开机时，将功放关闭
	
	ADC_check_result = GetADCResult(6);	//上电时，电池电量检测一次
	
 	comm_whole_control = 1;			//开启通信
	stolen_alarm_count = 0;			//清报警计数器
	stolen_alarm_flag = 0;			//清报警标志

	Transmitter_EN = 0;		//关闭发射机

	while(1)
	{
		if(stolen_alarm_flag == 1)		//编码3开始相应的报警
		{
			if(stolen_alarm_count < 2)
			{
				Alarm_stolen_speech();		//提示被盗
				Moto_Vibration();          //马达振动
				stolen_alarm_count++;
			}
		}

		if(raised_alarm_flag == 1)		//抬起开始相应的报警
		{
			Alarm_raised_speech();		//提示被抬起
			Moto_Vibration();          //马达振动
			raised_alarm_flag = 0;
		}

		if(fell_alarm_flag == 1)
		{
			Alarm_fell_speech();		   //提示倒下
			Moto_Vibration();          //马达振动
			fell_alarm_flag=0;
		}

		if(battery_check == 1)				//每个3s检测一次电量，如果电池满的就检测是否低了，如果是不满的就检测是否充满。
		{
			ADC_check_result = GetADCResult(6);	//电池电量检测
			battery_check = 0;
			
			if((battery_HV_flag == 1)&&(ADC_check_result <= 0x35a))
			{
				battery_HV_flag = 0;
				Battery_low_alarm_speech();		//电量不足语音提示
			}
			else if((battery_HV_flag == 0)&&(ADC_check_result >= 0x377))
			{
				battery_HV_flag = 1;
				Battery_high_alarm_speech();
			}	
		}

		if(transmit_comm1_flag == 1)
		{
			Receiver_EN = 0;		//打开接收机
			ComMode_1_Data();	//发送模式1信号
			Receiver_EN = 1;		//打开接收机
			transmit_comm1_flag = 0;
		}

		if(Host_battery_high_flag == 1)
		{
         Host_battery_high_alarm_speech();		//提示主机电已经充满
			Host_battery_high_flag = 0;
		}
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/