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
//	myPwm();			//开发射机

	Moto_EN = 1;		//初始化，关闭马达
	transmit_wire = 1;
	Voice_EN = 0;		  	//开机时，将功放关闭
	
	ADC_check_result = GetADCResult(6);	//上电时，电池电量检测一次
	
	stolen_alarm_count = 0;			//清报警计数器
	stolen_alarm_flag = 0;			//清报警标志

	Transmitter_EN = 1;		// turn off the transmitter
	Receiver_EN = 0;			// turn on the receiver
   Delay(3);
	battery_stolen_speech();
	while(1)
		{
		sEOS_Go_To_Sleep();
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/