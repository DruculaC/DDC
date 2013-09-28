/*---------------------------------------------------
	Battery.c (v1.00)
	
	��ص����ж�
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Battery.h"
#include "voice.h"
#include "Delay.h"

/*---------------------------------------------------*/


/*----------------------------------------------------
	verifybattery()
	
	�жϵ�ص�����Ȼ����е�����ʾ
-----------------------------------------------------*/

void verifybattery(unsigned int Check2)
{									  
	if((Check2>=0x3a3))//���ñȽϵ�ѹ���˴���4.96VΪ��׼������47.4V��4.5V��
	{
		PAshutdown=1;
		SC_Speech(6);  	//4V����������ʾ
		Delay(130);
		PAshutdown=0;
	}
    else if((Check2<0x3a0)&&(Check2>=0x37c))		//С��47.4������45��4.31V��
	{
		PAshutdown=1;
		SC_Speech(5);  //3.8V����������ʾ
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x378)&&(Check2>=0x359))		 //С��45������43��4.14V��
	{
		PAshutdown=1;
		SC_Speech(4);  //3.6V����������ʾ
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x355)&&(Check2>=0x327))		 //С��43������40��3.9V��
	{
		PAshutdown=1;
		SC_Speech(3);  //����3.6v����������ʾ
		Delay(130);
		PAshutdown=0;
	}
	else if((Check2<0x323)&&(Check2>=0x304))		  //С��40������38��3.73V��
	{
		PAshutdown=1;
		SC_Speech(2);  //����3.6v����������ʾ
		Delay(130);
		PAshutdown=0;
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/