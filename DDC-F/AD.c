/*---------------------------------------------------
	AD.c (v1.00)
	
	adc functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "ad.h"

/*----------------------------------------------------
	GetADCResult(tByte ch)
	通过指定通道，来生成ADC的值。
----------------------------------------------------*/

tWord GetADCResult(tByte ch)
	{
	tByte DataL=0;
	tWord DataH=0;
	
	if(ch==5)	//检测拾音器的电量大小
		{
		P0DIDS|=0x20;	// Set ADC0 (P0.5 default) is input only mode

		AADR0=0;	//选择通道P0.5
		AADR1=0;
		AADR2=1;
	                                                                  
	   ADCCON1|=0x80;                                          // Enable ADC Function   
		
		ADCI=0;                                           // Clear ADC flag (ADCI=0)
    	ADCS=1;  
		
		DataL=ADCCON0;
		DataL=DataL>>6; 

		DataH=ADCH;
		DataH=(DataH<<2)+DataL;

		return DataH;
		}
	else if(ch==6)	//电池检测
		{
		P0DIDS|=0x40;	// Set ADC0 (P0.6 default) is input only mode

		AADR0=1;	//选择通道P0.5
		AADR1=0;
		AADR2=1;
	                                                                  
	    ADCCON1|=0x80;                                          // Enable ADC Function   
		
		ADCI=0;                                           // Clear ADC flag (ADCI=0)
    	ADCS=1;  
		
		DataL=ADCCON0;
		DataL=DataL>>6; 

		DataH=ADCH;
		DataH=(DataH<<2)+DataL;

		return DataH;
		}
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/    

