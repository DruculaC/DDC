/*---------------------------------------------------
	voice.c (v1.00)
	
	������ʾ�ӳ���
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "voice.h"
#include "Delay.h"
#include "communication.h"
   
/*--------------------------------------------------
	SC_Speech()
	
	��������оƬ������������ʾ�����������ʾ���ŵ�
	����������ʾ
---------------------------------------------------*/

void SC_Speech(unsigned char cnt)
{
	unsigned char i;
//	SC_RST=1;
//	delay_ms(15); //DAC, ���� 32 ��Ϊ 15MS
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // ���������
//		delay_us(350); // ��ʱ 100US
		delay_us(30);
		SC_DATA=0; // ���������
//		delay_us(350); // ��ʱ 100US
		delay_us(30);
	}
}

/*--------------------------------------------------
	SC_Speech2()
	
	��������оƬ������������ʾ�����������ʾ���ŵ�
	����������ʾ
---------------------------------------------------*/

void SC_Speech2(unsigned char cnt)
{
	unsigned char i;
//	SC_RST=1;
//	delay_ms(15); //DAC, ���� 32 ��Ϊ 15MS
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // ���������
//		delay_us(350); // ��ʱ 100US
		delay_us(30);
		SC_DATA=0; // ���������
//		delay_us(350); // ��ʱ 100US
		delay_us(30);
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
	PAshutdown=1;
	SC_Speech(7);  
	Delay(160);
	PAshutdown=0;
	}
	
/*-----------------------------------------------------
	key_rotate_off_speech()
	Կ��ת����off��ʱ��������ʾ
------------------------------------------------------*/
void key_rotate_off_speech(void)
	{
	PAshutdown=1;
	SC_Speech(9);
	Delay(130);
	PAshutdown=0;
	}

/*----------------------------------------------------
	motorBAT_low_speech()
	��ƿ����������С�޶�ֵ��������ʾ��
-----------------------------------------------------*/
void motorBAT_low_speech(void)
	{
	PAshutdown=1;
	SC_Speech(10);
	Delay(100);
	PAshutdown=0;
	}
	
/*----------------------------------------------------
	stolen_alarm_speech()
	�ж�Ϊ������ʱ�򣬷�����������
----------------------------------------------------*/
void stolen_alarm_speech(void)
	{
	PAshutdown=1;
	SC_Speech(22); 
	ComMode_3_Data();
	Delay(100);
	SC_Speech(23); 
	ComMode_3_Data();
	Delay(60);
	PAshutdown=0; 
	}

/*----------------------------------------------------
	slave_nearby_speech()
	�����ӵ������źź󣬱�������֪�������ڸ�������ʱ
	��һ��������
-----------------------------------------------------*/
void slave_nearby_speech(void)
	{
	PAshutdown=1;
	SC_Speech(11); 
	Delay(150);
	PAshutdown=0;
	}

/*----------------------------------------------------
	slave_away_speech()
	�����Ӳ��������źź󣬱���������Ϊ�����뿪�ˣ���ʱ
	��һ������
-----------------------------------------------------*/
void slave_away_speech(void)
	{
	PAshutdown=1;
	SC_Speech(12);  
	Delay(80);
	SC_Speech(13);
	Delay(80);
	PAshutdown=0;
	}

/*----------------------------------------------------
	host_touch_speech()
	��һ��������������һ����ʾ����
--------------------------------------------------------*/
void host_touch_speech(void)
	{
	PAshutdown=1;
	SC_Speech2(17);  //�ػ���������
	Delay(80);
	PAshutdown=0;
	}
/*---------------------------------------------------
	end of file
----------------------------------------------------*/