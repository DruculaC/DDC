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
extern bit fell_alarm_flag;				//���ر�����־
extern bit raised_alarm_flag;				//̧�𱨾���־
extern bit stolen_alarm_flag;				//����������־
extern tWord stolen_alarm_count;     	//��������ѭ������
extern bit comm_whole_control;			//ͨ���ܿ��أ�0�ر�ͨ�ţ�1��ͨ��
extern bit transmit_comm1_flag;			//ÿ��һ��ʱ�佫����1������main�����з������ݣ��Ͳ���Ӱ��timer0�ļ�ʱ�������������0.
extern bit battery_check;					//��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
extern bit Host_battery_high_flag;		//��1ʱ��ִ��һ��������ʾ����ʾ�������

/*------- Private variable declarations --------------------------*/
tWord ADC_check_result = 0;		//��ΪAD���ֵ
bit battery_HV_flag = 1;			//��ص�λ�ı�ǣ�1��ʾ���ڵ�������ģ�0��ʾ��û����

void main(void)
{
	noVoice();
	InitTimer(50, 100);
	myPwm();			//�������

	Moto_EN = 1;		//��ʼ�����ر����
	transmit_wire = 1;
	Voice_EN = 0;		  	//����ʱ�������Źر�
	
	ADC_check_result = GetADCResult(6);	//�ϵ�ʱ����ص������һ��
	
 	comm_whole_control = 1;			//����ͨ��
	stolen_alarm_count = 0;			//�屨��������
	stolen_alarm_flag = 0;			//�屨����־

	Transmitter_EN = 0;		//�رշ����

	while(1)
	{
		if(stolen_alarm_flag == 1)		//����3��ʼ��Ӧ�ı���
		{
			if(stolen_alarm_count < 2)
			{
				Alarm_stolen_speech();		//��ʾ����
				Moto_Vibration();          //�����
				stolen_alarm_count++;
			}
		}

		if(raised_alarm_flag == 1)		//̧��ʼ��Ӧ�ı���
		{
			Alarm_raised_speech();		//��ʾ��̧��
			Moto_Vibration();          //�����
			raised_alarm_flag = 0;
		}

		if(fell_alarm_flag == 1)
		{
			Alarm_fell_speech();		   //��ʾ����
			Moto_Vibration();          //�����
			fell_alarm_flag=0;
		}

		if(battery_check == 1)				//ÿ��3s���һ�ε��������������ľͼ���Ƿ���ˣ�����ǲ����ľͼ���Ƿ������
		{
			ADC_check_result = GetADCResult(6);	//��ص������
			battery_check = 0;
			
			if((battery_HV_flag == 1)&&(ADC_check_result <= 0x35a))
			{
				battery_HV_flag = 0;
				Battery_low_alarm_speech();		//��������������ʾ
			}
			else if((battery_HV_flag == 0)&&(ADC_check_result >= 0x377))
			{
				battery_HV_flag = 1;
				Battery_high_alarm_speech();
			}	
		}

		if(transmit_comm1_flag == 1)
		{
			Receiver_EN = 0;		//�򿪽��ջ�
			ComMode_1_Data();	//����ģʽ1�ź�
			Receiver_EN = 1;		//�򿪽��ջ�
			transmit_comm1_flag = 0;
		}

		if(Host_battery_high_flag == 1)
		{
         Host_battery_high_alarm_speech();		//��ʾ�������Ѿ�����
			Host_battery_high_flag = 0;
		}
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/