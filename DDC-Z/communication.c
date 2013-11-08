/*---------------------------------------------------
	communication.c (v1.00)
	
	ͨ�ų���
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "communication.h"
#include "Delay.h"

/*-------------------------------------------------*/
unsigned char myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};


/*----------------------------------------------------
	initsignal()
	
	��ʼ���źų��򣬽��ջ��ڽӽ����źŵ�ʱ����Ҫ��
	һ�δ�����ʹ���ջ��򿪣�����һ�γ����п����޷�ʶ��
	�����������൱�ڷϴ��룬ֻռ��ͨ��ʱ�䡣
----------------------------------------------------*/

void initsignal()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(80);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			mystartbuffer<<=1;
			Delay3(150);//��ʱҪ����2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal2()
	
	��ʼ���źų��򣬽��ջ��ڽӽ����źŵ�ʱ����Ҫ��
	һ�δ�����ʹ���ջ��򿪣�����һ�γ����п����޷�ʶ��
	�����������൱�ڷϴ��룬ֻռ��ͨ��ʱ�䡣
----------------------------------------------------*/

void initsignal2()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(80);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			mystartbuffer<<=1;
			Delay3(150);//��ʱҪ����2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal3()
	
	��ʼ���źų��򣬽��ջ��ڽӽ����źŵ�ʱ����Ҫ��
	һ�δ�����ʹ���ջ��򿪣�����һ�γ����п����޷�ʶ��
	�����������൱�ڷϴ��룬ֻռ��ͨ��ʱ�䡣
----------------------------------------------------*/

void initsignal3()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(80);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			mystartbuffer<<=1;
			Delay3(150);//��ʱҪ����2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*--------------------------------------------------
	ComMode_1_Data()
	
	�������յ�����1�źź󣬻ᷴ��һ������1�źŸ�����
	�Ա�ʾ�����ڸ���������
---------------------------------------------------*/

void ComMode_1_Data()	
{
	unsigned char i,n;
	transmiter_power=0;				
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal2();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay4(120);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay4(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			myTxRxData[i]<<=1;
			Delay4(50);//��ʱҪ����2ms
		}
	}
	transmiter_EN=0;
}

/*----------------------------------------------------------
	ComMode_3_Data()
	
	���������źţ��ø���������ʾ���綯��������
----------------------------------------------------------*/

void ComMode_3_Data()
{
//	unsigned int j;
	unsigned char i,n;
	transmiter_power=1;//�л�Ϊ300M����
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_3;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(120);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			myTxRxData[i]<<=1;
			Delay3(50);//��ʱҪ����2ms
		}
	}
	transmiter_EN=0;
}

/*--------------------------------------------------------
	ComMode_4_Data()
	
	���źŸ�����������������ʾ���綯����̧��
---------------------------------------------------------*/

void ComMode_4_Data()//����̧�����
{
	unsigned char i,n;
	transmiter_power=0;//�л�Ϊ300M����
	transmiter_EN=1;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_4;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal3();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(120);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			myTxRxData[i]<<=1;
			Delay3(50);//��ʱҪ����2ms
		}
	}
	transmiter_EN=0;
}

/*-----------------------------------------------------------
	ComMode_5_Data()
	
	���źŸ�����������������ʾ���綯�������ˡ�
------------------------------------------------------------*/

void ComMode_5_Data()//���͵��ر���
{
	unsigned char i,n;
	transmiter_power=0;//�л�Ϊ300M����
	transmiter_EN=1;      //�����߷����
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_5;
/*	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
*/
	initsignal3();

	for(i=0;i<3;i++)
	{
		for(n=0;n<8;n++)
		{
			if((myTxRxData[i]&0x80)==0x80)//Ϊ1
			{
				P10=0;
				Delay3(120);//��ʱ4.5ms���ϣ����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			else//Ϊ0�����
			{
				P10=0;
				Delay3(80);//��ʱ2ms�����ڶ�ʱ��ռ�����⣬ֻ����������ʱ��ʵ��
			}
			P10=1;//��̬Ϊ�ߵ�ƽ
			myTxRxData[i]<<=1;
			Delay3(50);//��ʱҪ����2ms
		}
	}
	transmiter_EN=0;
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/