/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//�綯��
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
extern bit stolen_alarm_flag;		//��������������־��1��ʱ���ʾ����		
extern bit host_stolen_speech_EN;		//������������ʹ�ܣ�1��ʱ����main��������ʾ
extern bit battery_check;		         //��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
extern bit position_sensor_EN;  			//λ�ô�������������̧�𴫸����ܿ��أ�1��ʱ�򣬼��������������
extern bit slave_away_speech_EN;      	//�жϸ����뿪��������ʾ����main�������
extern bit magnet_CW_EN;					//�����˳ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
extern bit comm_whole_control;				//ͨ���ܿ��أ�0�ر�ͨ�ţ�1��ͨ��
extern tWord host_stolen_speech_count;    //�ж�����������������ʾ�Ĵ���
extern bit magnet_ACW_EN;						//�������ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
extern bit slave_nearby_speech_EN;       //�жϸ���������������ʾ����main�������

/*------- Private variable declarations --------------------------*/
bit magnet_ACW_flag=0;
tByte slave_nearby_speech_count=0;
tByte slave_away_speech_count=0;
bit key_rotated_on_flag=0;			//�綯�������رձ�־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
tWord ADC_check_result = 0;		//��ΪAD���ֵ

void main()
{
	InitTimer(1,100);
	
	sensor_EN=0;

	noVoice();
	myPwm();	//�������

	voice_EN=0;		//�����Źر�

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;

	transmiter_power=0; //�����ģʽ���ƶ�,����ʱΪ30Mģʽ
	
	magnet_ACW_flag=0;

	comm_whole_control=1; //����ͨ��
	transmiter_EN=0;

	position_sensor_EN=1;
	key_rotate = 1;

	while(1)
	{
		if((key_rotate==1)&&(key_rotated_on_flag==0))		   //����ת��Կ��ʱ��ִ��һ�ε���ת��
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
				if((ADC_check_result<0x300))						  //С��38V������С��5������
					{
					motorBAT_low_speech();
					}
					
				key_rotate_off_speech();
				key_rotated_on_flag=0;
			}
		}

		if(battery_check==1)
		{
			ADC_check_result=GetADCResult(6);	//��ص������
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
				sensor_EN=1;	//�������ᴫ����
		}
	}
}



/*---------------------------------------------------
	end of file
----------------------------------------------------*/