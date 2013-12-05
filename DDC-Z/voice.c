/*---------------------------------------------------
	voice.c (v1.00)
	
	������ʾ�ӳ���
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "voice.h"
#include "Delay.h"
#include "communication.h"

/*------ private variable --------------------------*/
tByte key_rotate_on_speech_number = 1;
   
/*--------------------------------------------------
	SC_Speech()
	
	��������оƬ������������ʾ�����������ʾ���ŵ�
	����������ʾ
---------------------------------------------------*/

void SC_Speech(tByte cnt)
{
	tByte i;
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // ���������
		delay_us(250);
		SC_DATA=0; // ���������
		delay_us(250);
	}
}

/*--------------------------------------------------
	SC_Speech2()
	
	��������оƬ������������ʾ�����������ʾ���ŵ�
	����������ʾ
---------------------------------------------------*/

void SC_Speech2(tByte cnt)
{
	tByte i;
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // ���������
		delay_us(250);
		SC_DATA=0; // ���������
		delay_us(250);
	}
}

/*-------------------------------------------------
	noVoice()
	
	���ϵ��ʱ����ã�������������
--------------------------------------------------*/

void noVoice()
{
	P14=0;
	SC_DATA=0;
	SC_RST=0;
	delay_us(100); // �ϵ縴λ��ֹ���ŷ���
	SC_RST=1;
	delay_us(100);
	P14=1;
}

/*----------------------------------------------------
	key_rotate_on_speech()
	Կ��ת����on��ʱ��������ʾ
-----------------------------------------------------*/
void key_rotate_on_speech(void)
	{
	switch(key_rotate_on_speech_number)
		{
		case 1:
			{
			voice_EN=1;
			SC_Speech(10);  
			Delay(20);
			SC_Speech(11);  
			Delay(20);
			SC_Speech(6);  
			Delay(80);
			voice_EN=0;
         key_rotate_on_speech_number = 2;
			}
		break;
		
		case 2:
			{
			voice_EN=1;
			SC_Speech(10);  
			Delay(20);
			SC_Speech(8);  
			Delay(60);
			voice_EN=0;
         key_rotate_on_speech_number = 3;			
			}
		break;
		
		case 3:
			{
			voice_EN=1;
			SC_Speech(10);  
			Delay(20);
			SC_Speech(11);  
			Delay(20);
			SC_Speech(7);  
			Delay(60);
			voice_EN=0;
         key_rotate_on_speech_number = 1;			
			}
		break;
		}
	}
	
/*-----------------------------------------------------
	key_rotate_off_speech()
	Կ��ת����off��ʱ��������ʾ
------------------------------------------------------*/
void key_rotate_off_speech(void)
	{
	voice_EN=1;
	SC_Speech(9);
	Delay(60);
	voice_EN=0;
	}

/*----------------------------------------------------
	motorBAT_low_speech()
	��ƿ����������С�޶�ֵ��������ʾ��
-----------------------------------------------------*/
void motorBAT_low_speech(void)
	{
	voice_EN=1;
	SC_Speech(5);
	Delay(60);
	voice_EN=0;
	}

/*----------------------------------------------------
	host_touch_speech()
	��һ��������������һ����ʾ����
--------------------------------------------------------*/
void host_touch_speech(void)
	{
	voice_EN=1;
	SC_Speech2(2);  
	Delay(60);
	voice_EN=0;
	}

/*-----------------------------------------------------
	host_2ndtouch_speech()
	�ڶ��δ����������ٱ�һ����ʾ����
-------------------------------------------------------*/
void host_2ndtouch_speech(void)
	{
	voice_EN=1;
	SC_Speech2(14);  
	Delay(150);
	voice_EN=0;
	}
	
/*----------------------------------------------------
	stolen_alarm_speech()
	�ж�Ϊ������ʱ�򣬷�������������һ��
----------------------------------------------------*/
void stolen_alarm_speech1(void)
	{
	voice_EN=1;
	SC_Speech(15); 
	ComMode_3_Data();
	Delay(120);
	SC_Speech(13); 
	ComMode_3_Data();
	Delay(60);
	voice_EN=0; 
	}

/*----------------------------------------------------
	stolen_alarm_speech()
	�ж�Ϊ������ʱ�򣬷������������ڶ���
----------------------------------------------------*/
void stolen_alarm_speech2(void)
	{
	voice_EN=1;
	SC_Speech(3); 
	ComMode_3_Data();
	Delay(120);
	SC_Speech(13); 
	ComMode_3_Data();
	Delay(60);
	voice_EN=0; 
	}

/*----------------------------------------------------
	slave_nearby_speech()
	�����ӵ������źź󣬱�������֪�������ڸ�������ʱ
	��һ��������
-----------------------------------------------------*/
void slave_nearby_speech(void)
	{
	voice_EN=1;
	SC_Speech(16); 
	Delay(50);
	voice_EN=0;
	}

/*----------------------------------------------------
	slave_away_speech()
	�����Ӳ��������źź󣬱���������Ϊ�����뿪�ˣ���ʱ
	��һ������
-----------------------------------------------------*/
void slave_away_speech(void)
	{
	voice_EN=1;
	SC_Speech(17);  
	Delay(50);
	SC_Speech(4);  
	Delay(60);
	voice_EN=0;
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/