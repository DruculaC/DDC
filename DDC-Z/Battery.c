/*---------------------------------------------------
	Battery.c (v1.00)
	
	电池电量判断
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"

/*---------------------------------------------------*/


/*----------------------------------------------------
	verifybattery()
	
	判断电池电量，然后进行电量提示
-----------------------------------------------------*/

void verifybattery(unsigned int Check2)
{									  
	if((Check2>=0x3a3))//设置比较电压，此处以4.96V为基准，大于47.4V（4.5V）
	{
		PAshutdown=1;
		SC_Speech(6);  	//4V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
    else if((Check2<0x3a0)&&(Check2>=0x37c))		//小于47.4，大于45（4.31V）
	{
		PAshutdown=1;
		SC_Speech(5);  //3.8V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x378)&&(Check2>=0x359))		 //小于45，大于43（4.14V）
	{
		PAshutdown=1;
		SC_Speech(4);  //3.6V电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x355)&&(Check2>=0x327))		 //小于43，大于40（3.9V）
	{
		PAshutdown=1;
		SC_Speech(3);  //低于3.6v电量充足提示
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x323)&&(Check2>=0x304))		  //小于40，大于38（3.73V）
	{
		PAshutdown=1;
		SC_Speech(2);  //低于3.6v电量充足提示
		Delay(130);
		PAshutdown=0;
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/