/*---------------------------------------------------
	Delay.c (v1.00)
	
	Delay functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "AD.h"
#include "Delay.h"

/*----------------------------------------------------
	GetADCResult(tByte ch)
	ͨ��ָ��ͨ����������ADC��ֵ��
----------------------------------------------------*/

unsigned int GetADCResult(unsigned char ch)
{
	unsigned char DataL=0;
	unsigned int DataH=0;
	if(ch==5)//���ʰ�����ĵ�����С
	{
		P0DIDS|=0x20; // Set ADC0 (P0.5 default) is input only mode

		AADR0=0;	//ѡ��ͨ��P0.5
		AADR1=0;
		AADR2=1;
	                                                                  
	    ADCCON1|=0x80;                                          
		
		ADCI=0;                                           
    	ADCS=1;  
		
		DataL=ADCCON0;
		DataL=DataL>>6; 

		DataH=ADCH;
		DataH=(DataH<<2)+DataL;

		return DataH;
	}
	else if(ch==6) //��ؼ��
	{
		P0DIDS|=0x40; // Set ADC0 (P0.6 default) is input only mode

		AADR0=1;	//ѡ��ͨ��P0.6
		AADR1=0;
		AADR2=1;
	                                                                  
	    ADCCON1|=0x80;                                      
		
		ADCI=0;                                       
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