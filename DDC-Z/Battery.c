/*---------------------------------------------------
	Battery.c (v1.00)
	
	电池电量判断
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"

/*------ public variable -----------------------------------*/
extern tByte key_rotated_on_flag;

/*----------------------------------------------------
	verifybattery()
	voice alarm for battery status
-----------------------------------------------------*/

void verifybattery(tWord Check2)
	{	
	// battery is sufficient, battery > 54.6V, 3.18V/3.67V
	if((Check2 >= 0x377))
		{
		voice_EN = 1;
		SC_Speech(28);  	
		Delay(50);
		voice_EN = 0;
		}
   // 20km, 52V < battery < 54.6, 3.08V/3.67V
	else if((Check2 < 0x374)&&(Check2 >= 0x35B))		
		{
		voice_EN = 1;
		SC_Speech(10);  
		Delay(80);
		voice_EN = 0;
		}
	// 15km, 50V < battery < 52V, 2.98V/3.65V 
	else if((Check2 < 0x358)&&(Check2 >= 0x344))		
		{
		voice_EN = 1;
		SC_Speech(9);  	
		Delay(80);
		voice_EN = 0;
		}
	// 10km, 48V < battery <50V, 2.88V/3.64V
	else if((Check2 < 0x341)&&(Check2 >= 0x32a))		
		{
		voice_EN = 1;
		SC_Speech(8);  	
		Delay(80);
		voice_EN = 0;
		}
	// 5km, 46V < battery < 48V, 2.78V/3.63V
	else if((Check2 < 0x327)&&(Check2 >= 0x310))		
		{
		voice_EN =1;
		SC_Speech(7);  	//4V电量充足提示
		Delay(80);
		voice_EN = 0;
		}
	// battery is low, battery < 46V
	else if((Check2 < 0x30d)&&(key_rotated_on_flag == 1))
		{
		motorBAT_low_speech();
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/