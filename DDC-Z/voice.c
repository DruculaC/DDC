/*---------------------------------------------------
	voice.c (v1.00)
	
	include the voice function
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
	
	control the voice IC, need which seg, so send how many pulses.
---------------------------------------------------*/

void SC_Speech(tByte cnt)
	{
	tByte i;
	SC_RST = 0;
	delay_ms(100);
	SC_RST = 1;
	delay_ms(150);
	for(i=0; i < cnt; i++)
		{
		SC_DATA = 1; 
		delay_us(20);
		SC_DATA = 0; 
		delay_us(20);
		}
	}

/*--------------------------------------------------
	SC_Speech2()
	
	control the voice IC, need which seg, so send how many pulses.

void SC_Speech2(tByte cnt)
{
	tByte i;
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // 数据脉冲高
		delay_us(350);
		SC_DATA=0; // 数据脉冲低
		delay_us(350);
	}
}
---------------------------------------------------*/

/*-------------------------------------------------
	noVoice()
	
	在上电的时候调用，屏蔽语音的误报
--------------------------------------------------*/

void noVoice()
	{
	P14=0;
	SC_DATA = 0;
	SC_RST = 0;
	delay_us(350); // 上电复位防止干扰发声
	SC_RST = 1;
	delay_us(350);
	P14 = 1;
	}

/*----------------------------------------------------
	key_rotate_on_speech()
	speech for key rotated on
-----------------------------------------------------*/
void key_rotate_on_speech(void)
	{
	switch(key_rotate_on_speech_number)
		{
		case 1:
			{
			voice_EN = 1;
			SC_Speech(14);  
			Delay(30);
			SC_Speech(18);  
			Delay(30);
			SC_Speech(11);  
			Delay(50);
			voice_EN = 0;
         key_rotate_on_speech_number = 2;
			}
		break;
		
		case 2:
			{
			voice_EN = 1;
			SC_Speech(14);  
			Delay(25);
			SC_Speech(12);  
			Delay(50);
			voice_EN = 0;
         key_rotate_on_speech_number = 1;			
			}
		break;
		}
	}
	
/*-----------------------------------------------------
	key_rotate_off_speech()
	speech for key rotated off
void key_rotate_off_speech(void)
	{
	voice_EN = 1;
	SC_Speech(13);
	Delay(40);
	voice_EN = 0;
	}
------------------------------------------------------*/

/*----------------------------------------------------
	motorBAT_low_speech()
	电瓶电量低于最小限定值，语音提示。
-----------------------------------------------------*/
void motorBAT_low_speech(void)
	{
	voice_EN = 1;
	SC_Speech(6);
	Delay(60);
	voice_EN = 0;
	}

/*----------------------------------------------------
	host_touch_speech()
	第一次碰主机，会有一句提示语音
--------------------------------------------------------*/
void host_touch_speech(void)
	{
	voice_EN = 1;
	SC_Speech(2);  
	Delay(60);
	voice_EN = 0;
	}

/*-----------------------------------------------------
	host_2ndtouch_speech()
	第二次触碰主机，再报一个提示语音
-------------------------------------------------------*/
void host_2ndtouch_speech(void)
	{
	voice_EN = 1;
	SC_Speech(27);  
	Delay(70);
	voice_EN = 0;
	}
	
/*----------------------------------------------------
	stolen_alarm_speech()
	判断为被盗的时候，发出报警语音第一段
----------------------------------------------------*/
void stolen_alarm_speech1(void)
	{
	voice_EN = 1;
	SC_Speech(3); 
	Delay(160);
	voice_EN = 0; 
	}

/*----------------------------------------------------
	stolen_alarm_speech()
	判断为被盗的时候，发出报警语音第二段
----------------------------------------------------*/
void stolen_alarm_speech2(void)
	{
	voice_EN = 1;
	SC_Speech(22); 
	Delay(160);
	voice_EN = 0;
	Delay(20);
	}

/*----------------------------------------------------
	slave_nearby_speech()
	主机接到附机信号后，表明主机知道附机在附近，此时
	报一段语音。
-----------------------------------------------------*/
void slave_nearby_speech(void)
	{
	voice_EN=1;
	SC_Speech(23); 
	Delay(50);
	voice_EN=0;
	}

/*----------------------------------------------------
	slave_away_speech()
	主机接不到附机信号后，表明主机认为附机离开了，此时
	报一段语音
-----------------------------------------------------*/
void slave_away_speech(void)
	{
	voice_EN = 1;
	// 车锁已锁上
	SC_Speech(24);  
	Delay(50);
	// 祝您平安返回
	SC_Speech(13);
	Delay(50);
	voice_EN = 0;
	}

/*----------------------------------------------------
	battery_stolen_speech()
	主机检测到电平不在的时候，播报的语音
-----------------------------------------------------*/
void battery_stolen_speech(void)
	{
	voice_EN = 1;
	SC_Speech(19);  
	Delay(80);
	voice_EN = 0;
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/