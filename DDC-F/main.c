/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-F program, for electrocar.
----------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "AD.h"
#include "T0.h"
#include "voice.h"
#include "pwm.h"
#include "T1.h"
#include "Delay.h"
#include "communication.h"

/*--------------------------------------------------*/

bit ModeFlag=0;				//ģʽѡ��λ��0����ģʽ1,1����ģʽ2
bit receiveFlag=0;			//���յ����ݱ�־
bit commuFlag=0;			//����ͨ�ű�־

bit alarmFlag2=0;			//�Ƿ����������־
bit alarmFlag3=0;			//�Ƿ����������־
bit alarmFlag4=0;			//̧�𱨾�
bit alarmFlag5=0;			//���ر���
unsigned int alarmCount2=0;	//����2ѭ������
unsigned int alarmCount3=0;	//����3ѭ������
unsigned int alarmCount4=0;	//̧�𱨾�ѭ������
unsigned int alarmCount5=0;	//���ر���ѭ������

unsigned char voiceFlag=0;	//����ѭ������ 

unsigned char dataFirst=0;	//���ڴ洢�ϴα�������

unsigned char count=0;		//���ڽ��ղ��ֵļ�����
 
unsigned int time0Count_3=0;//��Ϊ����ÿ������������Ϣ����ʱ��

unsigned int lastAddr=0;	//��һ�ν��յ��ı���ĵ�ַ
unsigned int TestFlag=0;	//1��2��3�ֱ�Ϊÿ1S��ļ������ڴ��ڵĳɹ�ָ�����ִ�н�ȥ����Ĳ���
                			//�������3�ζ�û�й��㣬��˵�����ڳ���


unsigned int Check=0;		//��ΪAD���ֵ
bit checkFlag=0;			//��������־
 
unsigned char DataBetween=0;	//��Ϊ�������ݵ��м����
unsigned char RecData=0;		//���յ�������
unsigned char DataTime=0;		//��Ϊ���յ����ݵ���λ��������
bit ComFlag=1;					//�������ص�һ����־
unsigned char T1highcount=0;	//��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����Datatime��0

bit ADCcheck=0;			//��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
bit sendcomm1=0;		//��1ʱ��ִ��һ�η��ͱ���1��ִ����󣬽�����0
bit sendspeech1=0;		//��1ʱ��ִ��һ��������ʾ����ʾ�������
unsigned char speech1_count=0;	
unsigned char powerflag=1;		//��ص�λ�ı�ǣ�1��ʾ���ڵ�������ģ�0��ʾ��û����


tByte TxRxBuf[28]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
tByte myTxRxData2[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	//�������ݵĻ���

void main(void)
{
	noVoice();
	InitT0();
	InitT1();
	Moto=1;			//�ر����

	ET0=1;
	ET1=1;
	PT1=1;			//��ʱ��1���ж����ȼ����
	EA=1;

	myPwm();		//�������
	P10=1;
	PAshutdown=0;		  	//����ʱ�������Źر�
	
	Check=GetADCResult(6);	//�ϵ�ʱ����ص������һ��
	
 	commuFlag=1;			//����ͨ��
	alarmCount3=0;			//�屨��������
	alarmFlag3=0;			//�屨����־

//	receive_en=1;
	tran_en=0;

	while(1)
	{
		if(alarmFlag3==1)		//����3��ʼ��Ӧ�ı���
		{
			if(alarmCount3<2)
			{
				PAshutdown=1;
				SC_Speech(5);
				Delay(150);
				SC_Speech(6);
				Delay(80);
				SC_Speech(7);
				Delay(120);
				PAshutdown=0;
				
				Moto=0;				
				Delay(10);
				Moto=1;
				alarmCount3++;
			}
		}

		if(alarmFlag4==1)		//̧��ʼ��Ӧ�ı���
		{

				PAshutdown=1;
				SC_Speech(4);
				Delay(180);
				
				Moto=0;
				Delay(10);
				Moto=1;
				alarmFlag4=0;
		}

		if(alarmFlag5==1)//���ؿ�ʼ��Ӧ�ı���
		{
				PAshutdown=1;
				SC_Speech(8);
				Delay(180);
	
				Moto=0;//����
				Delay(10);
				Moto=1;

				alarmFlag5=0;
		}

		if(ADCcheck==1)				//ÿ��3s���һ�ε��������������ľͼ���Ƿ���ˣ�����ǲ����ľͼ���Ƿ������
		{
			Check=GetADCResult(6);	//��ص������
			ADCcheck=0;
			
			if((powerflag==1)&&(Check<=0x35a))
			{
				powerflag=0;
				PAshutdown=1;
				SC_Speech(9);	//��ѹ��������ʾ
				Delay(120);
				PAshutdown=0;
			}
			else if((powerflag==0)&&(Check>=0x377))
			{
				powerflag=1;
				PAshutdown=1;
				SC_Speech(2);	//��ѹ������ʾ
				Delay(120);
				PAshutdown=0;
			}	
		}

		if(sendcomm1==1)
		{
			receive_en=0;		//�򿪽��ջ�
			ComMode_1_Data();	//����ģʽ1�ź�
			receive_en=1;		//�򿪽��ջ�
			sendcomm1=0;
		}

		if(sendspeech1==1)
		{
			PAshutdown=1;
			SC_Speech(2);  
			Delay(80);
			SC_Speech(3);  
			Delay(80);
			PAshutdown=0;
			sendspeech1=0;
		}
	}
}

void timeT1() interrupt 3 				//��ʱ��1�жϽ�������
{
	TH1=timer1H;						//��װ��
	TL1=timer1L;

	if(P11==0)							//�������Ϊ�ߵ�ƽ,�е͵�ƽ˵�����ź�
	{
		DataBetween++;
		ComFlag=0;
		if(DataBetween==150)			//�͵�ƽ���������ʱ��	
		{
			DataBetween=0;
		}
	}
	else//Ϊ�ߵ�ƽ��
	{
		if(ComFlag==0)//˵����һ���͵�ƽ
		{
			ComFlag=1;
//			RecData<<=1;

			if((DataBetween>60)&&(DataBetween<=100))//�͵�ƽ������ʱ��С��3ms����Ϊ0
			{
				RecData<<=1;
				RecData &= 0xfe;
				DataTime++;
				T1highcount=0;
			}
			if(DataBetween>100)//�͵�ƽ������ʱ�����4.5ms����Ϊ1
			{
				RecData<<=1;
				RecData |= 0x01;
				DataTime++;
				T1highcount=0;
			}
			else
			{
				T1highcount++;	
			}

			DataBetween=0;
		}
		else
		{
			T1highcount++;
			if(T1highcount>=120)
			{
				DataTime=0;
				ComFlag=1;
				count=0;
			}		
		}
	}

	if(DataTime==8)//˵��һ���ֽڵ������Ѿ�������ȫ
	{
		DataTime=0;
		myTxRxData2[count]=RecData;
		if(count==0&&myTxRxData2[0]==CmdHead)
		{
			count=1;
		}
		else if(count==1&&myTxRxData2[1]==MyAddress)
		{
			count=2;
		}
		else if(count==2)
		{
			receiveFlag=1;
			count=0;
		}
		else 
		{
			count=0;
		}

/*
		else if(count>=2&&count<=5)
		{
			count++;
		}
		else if(count==6)
		{
			receiveFlag=1;
			count=0;
		}
		else 
		{
			count=0;
		}
*/
	}

	if(receiveFlag==1)	//˵�����յ������ݣ���ʼ����
	{
		receiveFlag=0;	//����ձ�־
		receive_en=0;			//�رս��ջ�
		switch(myTxRxData2[2])//����ָ��
		{
			case ComMode_1://���յ������������͹����ı���1���źţ�˵��������3M�ڣ���������
			{	
				TestFlag=0;//����������峬ʱ��־

				alarmCount3=0;//�屨��������
				alarmFlag3=0;//�屨����־

				alarmCount4=0;//�屨��������
				alarmFlag4=0;//�屨����־

				alarmCount5=0;//�屨��������
				alarmFlag5=0;//�屨����־

				Moto=0;//����
				Delay(10);
				Moto=1;
			}
			break;
		
			case ComMode_3:
			{
				TestFlag=0;//�峬ʱ��־				
				alarmFlag3=1;

				alarmCount4=0;//�屨��������
				alarmFlag4=0;//�屨����־
				alarmCount5=0;//�屨��������
				alarmFlag5=0;//�屨����־

				Moto=0;		//����
				Delay(10);
				Moto=1;
			}
			break;
		
			case ComMode_4://����̧���ź�ʹ��
			{
				TestFlag=0;//�峬ʱ��־	
				alarmFlag4=1;//̧�𱨾�

				alarmCount3=0;//�屨��������
				alarmFlag3=0;//�屨����־
				alarmCount5=0;//�屨��������
				alarmFlag5=0;//�屨����־

				Moto=0;		//����
				Delay(10);
				Moto=1;
			}
			break;

			case ComMode_5://���������ź�ʹ��
			{
				TestFlag=0;//�峬ʱ��־
				alarmFlag5=1;	//���ر���

				alarmCount3=0;//�屨��������
				alarmFlag3=0;//�屨����־
				alarmCount4=0;//�屨��������
				alarmFlag4=0;//�屨����־

				Moto=0;		//����
				Delay(10);
				Moto=1;
			}
			break;

			case ComMode_6://���������ź�ʹ��
			{
				sendspeech1=1;
			}
			break;
		}
	}
}

void time0() interrupt 1	//��Ϊ����ϵͳ�Լ���ʱ��
{
	TH0=timer0H;//��װ��
	TL0=timer0L;

	time0Count_3++;

	if(time0Count_3>=60)		//����ÿ1S����һ�ε����ݵ�ʱ���־
	{
		ADCcheck=1;
		if(commuFlag==1)		//˵��������ͨ��
		{
			TestFlag++;
			sendcomm1=1;
			if(TestFlag>=4)		//˵���Ѿ�����300M�ˡ��ղ����κ��ź��ˣ�Ҫ������
			{
				TestFlag=5;
			}
		}
		time0Count_3=0;
	}

	if(alarmCount3>=2)
	{
		alarmCount3++;
		if(alarmCount3==1800)
		{
			alarmCount3=0;
			alarmFlag3=0;
		}
	}
}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/