/*---------------------------------------------------
	voice.c (v1.00)
	
	语音提示子程序
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
	
	调用语音芯片，播报语音提示，几次脉冲表示播放第
	几段语音提示
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
		SC_DATA=1; // 数据脉冲高
		delay_us(250);
		SC_DATA=0; // 数据脉冲低
		delay_us(250);
	}
}

/*--------------------------------------------------
	SC_Speech2()
	
	调用语音芯片，播报语音提示，几次脉冲表示播放第
	几段语音提示
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
		SC_DATA=1; // 数据脉冲高
		delay_us(250);
		SC_DATA=0; // 数据脉冲低
		delay_us(250);
	}
}

/*-------------------------------------------------
	noVoice()
	
	在上电的时候调用，屏蔽语音的误报
--------------------------------------------------*/

void noVoice()
{
	P14=0;
	SC_DATA=0;
	SC_RST=0;
	delay_us(100); // 上电复位防止干扰发声
	SC_RST=1;
	delay_us(100);
	P14=1;
}

/*----------------------------------------------------
	key_rotate_on_speech()
	钥匙转动到on的时候，语音提示
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
	钥匙转动到off的时候，语音提示
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
	电瓶电量低于最小限定值，语音提示。
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
	第一次碰主机，会有一句提示语音
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
	第二次触碰主机，再报一个提示语音
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
	判断为被盗的时候，发出报警语音第一段
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
	判断为被盗的时候，发出报警语音第二段
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
	主机接到附机信号后，表明主机知道附机在附近，此时
	报一段语音。
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
	主机接不到附机信号后，表明主机认为附机离开了，此时
	报一段语音
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