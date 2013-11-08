/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//电动车
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "voice.h"
#include "pwm.h"
#include "Timer.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"
#include "Other.h"
                                        
/*------- Public variable declarations --------------------------*/
extern bit stolen_alarm_flag;		//主机被盗报警标志，1的时候表示触发		
extern bit host_stolen_speech_EN;		//主机被盗语音使能，1的时候在main中语音提示
extern bit battery_check;		         //置1时，执行一次电量转换，执行完后，将其置0
extern bit position_sensor_EN;  			//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
extern bit slave_away_speech_EN;      	//判断附机离开后，语音提示，在main里面操作
extern bit magnet_CW_EN;					//电磁铁顺时针转动使能，转动一次后复位为0
extern bit comm_whole_control;				//通信总开关，0关闭通信，1打开通信
extern tWord host_stolen_speech_count;    //判断主机被盗后，语音提示的次数
extern bit magnet_ACW_EN;						//电磁铁逆时针转动使能，转动一次后复位为0
extern bit slave_nearby_speech_EN;       //判断附近靠近后，语音提示，在main里面操作

/*------- Private variable declarations --------------------------*/
bit magnet_ACW_flag=0;
tByte slave_nearby_speech_count=0;
tByte slave_away_speech_count=0;
bit key_rotated_on_flag=0;			//电动车开启关闭标志位，1表示电动车开启了，0表示电动车关闭了
tWord ADC_check_result = 0;		//作为AD检测值

void main()
{
	InitTimer(1,100);
	
	sensor_EN=0;

	noVoice();
	myPwm();	//开发射机

	voice_EN=0;		//将功放关闭

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;

	transmiter_power=0; //发射机模式控制端,开机时为30M模式
	
	magnet_ACW_flag=0;

	comm_whole_control=1; //开启通信
	transmiter_EN=0;

	position_sensor_EN=1;
	key_rotate = 1;

	while(1)
	{
		if((key_rotate==1)&&(key_rotated_on_flag==0))		   //开车转动钥匙时，执行一次电量转换
		{
		 	Delay(30);
			if(key_rotate==1)
			{
				verifybattery(ADC_check_result);
				key_rotate_on_speech();
            key_rotated_on_flag=1;
			}
		}
		
		if((key_rotate==0)&&(key_rotated_on_flag==1))
		{
		 	Delay(30);
			if(key_rotate==0)
			{
				verifybattery(ADC_check_result);
				if((ADC_check_result<0x300))						  //小于38V报警，小于5公里了
					{
					motorBAT_low_speech();
					}
					
				key_rotate_off_speech();
				key_rotated_on_flag=0;
			}
		}

		if(battery_check==1)
		{
			ADC_check_result=GetADCResult(6);	//电池电量检测
			battery_check=0;	
		}

		if(magnet_CW_EN==1)
		{
			if(magnet_ACW_flag==1)
			{
            magnet_CW();
				magnet_ACW_flag=0;
			}
			magnet_CW_EN=0;
		}

		if(magnet_ACW_EN==1)
		{
			if(magnet_ACW_flag==0)
			{
				magnet_ACW();
				magnet_ACW_flag=1;
			}
			magnet_ACW_EN=0;
		}

		if((host_stolen_speech_EN==1)&&(host_stolen_speech_count<4))
		{
			if((raised_sensor_detect==1)&&(fell_sensor_detect==1))
				{
				stolen_alarm_speech();
				}
			host_stolen_speech_count++;
			if(host_stolen_speech_count==4)
			{
				host_stolen_speech_count=0;
				host_stolen_speech_EN=0;
				stolen_alarm_flag=0;
			}
		}

		if((slave_nearby_speech_EN==1)&&(slave_nearby_speech_count<1))
		{
				slave_away_speech_EN=0;
				slave_away_speech_count=0;
				slave_nearby_speech();
				slave_nearby_speech_count++;
		}

		if((slave_away_speech_EN==1)&&(slave_away_speech_count<1))
		{
				slave_nearby_speech_EN=0;
				slave_nearby_speech_count=0;
				
				slave_away_speech();
				
				slave_away_speech_count++;
				sensor_EN=1;	//开启三轴传感器
		}
	}
}



/*---------------------------------------------------
	end of file
----------------------------------------------------*/