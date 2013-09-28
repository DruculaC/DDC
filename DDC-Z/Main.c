/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//�綯��
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "T0.h"
#include "voice.h"
#include "pwm.h"
#include "T1.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"

/*---------------------------------------------------*/

unsigned char count=0;	//���ݽ��ղ��ֵļ�����

unsigned int lastAddr=0;//��һ�ν��յ��ı���ĵ�ַ

unsigned char time0Count_1=0;//��Ϊ���ᴫ������������֮���ʱ������ʱ
unsigned char time0Count_2=0;//��Ϊ���ᴫ�����ļ�ʱ
unsigned char time0Count_6=0;
unsigned int time0Count_3=0;//��Ϊ����ÿ������������Ϣ����ʱ��
unsigned char time0Count_4=0;//��Ϊ̧�������ʱ������ʱ
unsigned char time0Count_5=0;//��Ϊ���������ʱ������ʱ

bit SensorFlag=0; //���ᴫ�����ĵ͵�ƽ��־λ
unsigned char  SensorCount=0; //��Ϊ���ᴫ��������ļ���

unsigned char TestFlag=0;//1��2��3�ֱ�Ϊÿ�ν��յ��������������ݺ�ļ������ڴ��ڵĳɹ�ָ�����ִ�н�ȥ����Ĳ���
                //�������3�ζ�û�й��㣬��˵�����ڳ���
unsigned char ModeFlag=1;//ģʽѡ��λ��1����ģʽ1,2����ģʽ2,3��Ϊģʽ3

bit alarmFlag=0;//���������Ŀ�����־
bit alarmFlag2=0;//����������־2
//unsigned int alarmCount=0;//���������Ĵ���

bit threeFlag=0;//��·ѭ�����ر�־

bit power1Flag=0;
bit power2Flag=0;
bit power3Flag=0;
bit power4Flag=0;

bit voiceFlag=0;
bit downUpFlag=0;  //���غ�̧�����־

bit downFlag=0;//���صı�־
unsigned char downcount=0;
bit upFlag=0;//̧��ı�־
unsigned char upcount=0;
bit downFlagSend=0;//���ط��͵ı�־
bit upFlagSend=0;//̧���͵ı�־

//һ��ͷ�ֽڣ�һ����ַ�ֽڣ�һ�������ֽڣ����������ַ�ֽڣ���������
unsigned char myTxRxData2[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//unsigned char Check1=0;//��ΪAD���ֵ

bit receiveFlag=0;//���յ����ݱ�־
bit commuFlag=0;//����ͨ�ű�־

unsigned char DataBetween=0;//��Ϊ�������ݵ��м����
unsigned char RecData=0;//���յ�������
unsigned char DataTime=0;//��Ϊ���յ����ݵ���λ��������
bit ComFlag=1;//�������ص�һ����־
//unsigned int newAddr=0;
unsigned char T1highcount=0;	   //��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����Datatime��0


//��������ת�����
unsigned char magnetflag=0;


bit ADCcheck=0;			//��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
bit sendcomm1=0;		//��1ʱ��ִ��һ�η��ͱ���1��ִ����󣬽�����0
bit sendcomm3=0;		//��1ʱ��ִ��һ�η��ͱ���3��ִ����󣬽�����0
bit sendcomm4=0;		//��1ʱ��ִ��һ�η��ͱ���4��ִ����󣬽�����0
bit sendcomm5=0;		//��1ʱ��ִ��һ�η��ͱ���5��ִ����󣬽�����0
bit magcon=0;			//��1ʱ��ִ��һ�ε�������ϲ�����ִ����󣬽�����0
bit magcon2=0;			//��1ʱ��ִ��һ�ε�����򿪲�����ִ����󣬽�����0
bit sendspeech1=0;		//��1ʱ��ִ��һ�α��������������棩��ִ����󣬽�����0
unsigned char speech1_count=0;
bit sendspeech2=0;		//��1ʱ��ִ��һ�α�������������+��������ִ����󣬽�����0
bit sendspeech3=0;		//��1ʱ��ִ��һ�α��������������棩��ִ����󣬽�����0
unsigned int speech3_count=0;
bit sendspeech4=0;		//��1ʱ��ִ��һ�α�������������+��������ִ����󣬽�����0
bit sendspeech5=0;
bit sendspeech6=0;
bit sendspeech7=0;
unsigned char speech7_count=0;
bit sendspeech8=0;
unsigned char speech8_count=0;

bit stolenflag=0;		//������־λ
unsigned int stolen_count=0;	//����������ʱ��
unsigned char stolen_flag=0;	//��⴫������ʼ��־

unsigned char turnonflag=0;		//�綯�������رձ�־λ��1��ʾ�綯�������ˣ�0��ʾ�綯���ر���
unsigned char turnon_speech_flag=0;		//����������־λ�������任������

unsigned int Check=0;//�������

void main()
{
	SensorControl=0;		  //�ϵ�رմ�����

	noVoice();
	InitT0();
	InitT1();
	
	PAshutdown=0;		//�����Źر�

	upSignal=1;
	downSignal=1;
	ET0=1;	//������ʱ��0�ж�
	ET1=1; //������ʱ��1�ж�
	PT1=1;//��ʱ��1���ж����ȼ����
	EA=1;
	P10=1;
//	det_charge=1;

	BatteryControl=0;	//�����ϵ��ʱ����0�������Գ�磬�����û�г����������Ϊ�͵�ƽ
	myPwm();	//�������

	ModeControl_1=0; //�����ģʽ���ƶ�,����ʱΪ30Mģʽ
	
	magnetflag=0;

	commuFlag=1; //����ͨ��
	tran_en=0;   //�ر����߷����

	downUpFlag=1;
	det_battery=1;

	while(1)
	{
		if((det_battery==1)&&(turnonflag==0))		   //����ת��Կ��ʱ��ִ��һ�ε���ת��
		{
		 	Delay(30);
			if(det_battery==1)
			{
				verifybattery(Check);
				PAshutdown=1;
				SC_Speech(7);  
				Delay(160);
				PAshutdown=0;
				
/*				if(turnon_speech_flag==0)
				{
					PAshutdown=1;
					SC_Speech(8);  
					Delay(80);
					PAshutdown=0;
					turnon_speech_flag=1;
				}
				else
				{
					PAshutdown=1;
					SC_Speech(7);  
					Delay(240);
					PAshutdown=0;
					turnon_speech_flag=0;
				}
*/
				turnonflag=1;
			}
		}
		
		if((det_battery==0)&&(turnonflag==1))
		{
		 	Delay(30);
			if(det_battery==0)
			{
				verifybattery(Check);
				if((Check<0x300))						  //С��38V������С��5������
				{
					PAshutdown=1;
					SC_Speech(10);  //����3.6v����������ʾ
					Delay(100);
					PAshutdown=0;
				}
				PAshutdown=1;
				SC_Speech(9);
				Delay(130);
				PAshutdown=0;
				turnonflag=0;
			}
		}

		if(ADCcheck==1)
		{
			Check=GetADCResult(6);	//��ص������
			ADCcheck=0;	
		}

		if(magcon==1)
		{
			if(magnetflag==1)
			{
				MagentControl_1=1;//�رմ���
				MagentControl_2=0;
				Delay(40);
				MagentControl_1=0;//������̬Ϊ����ģʽ
				MagentControl_2=0;
				magnetflag=0;
			}
			magcon=0;
		}

		if(magcon2==1)
		{
			if(magnetflag==0)
			{
				MagentControl_1=0;//��������
				MagentControl_2=1;
				Delay(40);
				MagentControl_1=0;//������̬Ϊ����ģʽ
				MagentControl_2=0;
				magnetflag=1;
			}
			magcon2=0;
		}
/*
		if(sendspeech1==1)
		{
			PAshutdown=1;
			SC_Speech(17);  //�ػ���������
			Delay(80);
			PAshutdown=0;
			sendspeech1=0;
		}
*/
/*
		if(sendspeech2==1)
		{
			PAshutdown=1;
			SC_Speech(18);  //�ػ���������
			Delay(100);
			SC_Speech(16);  //�ػ���������
			Delay(100);
			PAshutdown=0;
			sendspeech2=0;
		}
*/
		if((sendspeech3==1)&&(speech3_count<4))
		{
			if((upSignal==1)&&(downSignal==1))
			{
				PAshutdown=1;
				SC_Speech(22);  //�ػ���������
				ComMode_3_Data();
				Delay(100);
				SC_Speech(23);  //�ػ���������		
				ComMode_3_Data();
				Delay(60);
				PAshutdown=0;
			}
			speech3_count++;
			if(speech3_count==4)
			{
				speech3_count=0;
				sendspeech3=0;
				stolenflag=0;
			}
		}

/*		if(sendspeech3==1)
		{
			PAshutdown=1;
			SC_Speech(18);  //�ػ���������
			Delay(120);
			SC_Speech(19);  //�ػ���������
			Delay(140);
			SC_Speech(20);  //�ػ���������
			Delay(170);
			PAshutdown=0;
			sendspeech3=0;
		}
*/
		if((sendspeech7==1)&&(speech7_count<1))
		{
				sendspeech8=0;
				speech8_count=0;
				
				PAshutdown=1;
				SC_Speech(11);  //�ػ���������
				Delay(150);
				PAshutdown=0;
				speech7_count++;
		}

		if((sendspeech8==1)&&(speech8_count<1))
		{
				sendspeech7=0;
				speech7_count=0;
				
				PAshutdown=1;
				SC_Speech(12);  
				Delay(80);
				SC_Speech(13);
				Delay(80);
				PAshutdown=0;
				speech8_count++;
				SensorControl=1;	//�������ᴫ����
		}
	}
}

void timeT1() interrupt 3 //��ʱ��1�жϽ�������
{

	TH1=timer1H;//��װ��
	TL1=timer1L;
	
	if(P11==0)//�������Ϊ�ߵ�ƽ,�е͵�ƽ˵�����ź�
	{
		DataBetween++;
		ComFlag=0;
		if(DataBetween==150)//�͵�ƽ���������ʱ��	
		{
			DataBetween=0;
		}
	}
	else//Ϊ�ߵ�ƽ��
	{
		if(ComFlag==0)//˵����һ���͵�ƽ
		{
			ComFlag=1;

			if((DataBetween>60)&&(DataBetween<=100))	//�͵�ƽ������ʱ��С��10ms����Ϊ0
			{
				RecData<<=1;
				RecData &= 0xfe;
				DataTime++;
				T1highcount=0;
			}
			else if((DataBetween>100))//�͵�ƽ������ʱ�����4.5ms����Ϊ1
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

	if(receiveFlag==1)
	{
		receiveFlag=0;
		switch(myTxRxData2[2]) 		//������֡���������д���
		{
			case ComMode_1:  		//�������͹�����ֻ��ģʽ1��˵�������������ģ����ݲ���Ϊ����ĵ�һ�͵ڶ����ֽڣ�Ϊ������ڵ��������Ŀ�ʼ�ֽڵ��Ǹ���ַ��Ȼ���������֡�������������ݷ��ͳ�ȥ
			{
//				sendcomm1=1;
				stolenflag=0;
				
				ComMode_1_Data(); 	//�򸽻����ͱ���3

				sendspeech7=1;		//����1��һ������

//				sendspeech8=0;
//				speech8_count=0;

				alarmFlag=0;		//�ر�����־λ
//				alarmCount=0;		//����������������
				SensorControl=0;	//���ᴫ����
				downUpFlag=0; 		//�ص��ء�̧����
				downFlag=0;
				upFlag=0;
				magcon2=1;			//�򿪵����
				
				SensorCount=0;
				time0Count_2=0;
				stolen_count=0;
				stolen_flag=0;
				sendspeech1=0;
				sendspeech2=0;
				sendspeech3=0;
				speech3_count=0;

				TestFlag=0;	
				if(ModeFlag==3||ModeFlag==2)
				{
					ModeFlag=1;
				}
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

	if(time0Count_3>=2000)//����ÿ3s����һ�ε����ݵ�ʱ���־
	{
		if(commuFlag==1)//˵��������ͨ��
		{
			TestFlag++;

			if(TestFlag>=4&&ModeFlag==1)//˵��û�н��յ������Ѿ���3���ˣ������Ѿ�����3M�����ھ�Ҫ�Ӵ��ʣ��л���ģʽ2,30M�ٿ��ܲ��ܽ��յ�����
			{
				TestFlag=5;
				if(ModeFlag==1)
				{
					magcon=1;		 	//���������
					sendspeech8=1;		//�������뿪����
					
//					sendspeech7=0;
//					speech7_count=0;

//					SensorControl=1;	//�������ᴫ����
					downUpFlag=1;		//�������ء�̧���־
					ModeFlag=2;

					SensorCount=0;
					time0Count_2=0;
				}	
 			}
 		}
		time0Count_3=0;
		
//		ComMode_1_Data();

		ADCcheck=1;		
		
		if((downFlag==1)&&(downcount<5))  //���غ�����Ӧ�Ķ���
		{
//			sendcomm5=1;
			ComMode_5_Data(); //�򸽻����ͱ���3
			downcount++;
		}
		if((upFlag==1)&&(upcount<5))		//̧�������Ӧ����
		{
//			sendcomm4=1;
			ComMode_4_Data(); //�򸽻����ͱ���3
			upcount++;
		}

/*
	if((stolenflag==1)&&(speech3_count<4))
		{
			ComMode_3_Data();	
		}
*/
	}

	if(SensorControl==1)	//������ᴫ�����Ƿ�򿪣����һ�û�б���
	{
		if((ReceWave==1)&&(stolenflag==0))
		{
			time0Count_2++;
			if(time0Count_2>=8)				 //ÿ1ms���һ�θߵ�ƽ�����������6ms�ĸ߶�ƽ��˵����������һ��
			{
				time0Count_2=0;
				SensorCount++;
				speech1_count=0;
				stolen_flag=1;
			}
		}
		else
		{
			time0Count_2=0;
		}
		
		if(stolen_flag==1)
		{
			stolen_count++;
			if(stolen_count>=6000)
			{
				SensorCount=0;
				time0Count_2=0;
				stolen_count=0;
				stolen_flag=0;
				sendspeech1=0;
				speech1_count=0;
				sendspeech2=0;
			}
		}
	}

	if(SensorCount==1)
	{
//		sendspeech1=1;
		if((speech1_count<1)&&(sendspeech3!=1))
		{
			if((downSignal==1)&&(upSignal==1))
			{
				PAshutdown=1;
				SC_Speech2(17);  //�ػ���������
				Delay(80);
				PAshutdown=0;
				speech1_count++;
			}
		}
	}
	else
	{
		if(stolen_count>=3000)
		{
			if(ReceWave==1)
			{
				time0Count_6++;
				if(time0Count_6>=6)	
				{
					sendspeech3=1;
					speech3_count=0;
					stolenflag=1;
					time0Count_6=0;	
				}
			}
			else
			{
				time0Count_6=0;
			}
		}						 
	}

//	��⵹�غ�̧����Ĵ���
	if(downUpFlag==1)//������̧�𵹵ؼ��
	{
		if(upSignal==0)//˵����̧���źŲ����ǵ�һ�Σ���ʼ��ʱ
		{
			time0Count_4++;
			if(time0Count_4==10)//˵���Ѿ�����0.5S
			{
				upFlag=1;//��̧���־
				downFlag=0;
				alarmFlag=0;
			}		
		}
		else
		{
			upFlag=0;
			upcount=0;
			time0Count_4=0;
		}

		if(downSignal==0)//˵����̧���źţ���ʼ��ʱ
		{
			time0Count_5++;
			if(time0Count_5==10)//˵���Ѿ�����0.5S
			{
				downFlag=1;//��̧���־
				upFlag=0;
				alarmFlag=0;
			}		
		}
		else
		{
			downFlag=0;
			downcount=0;
			time0Count_5=0;
		}
	}
}


/*---------------------------------------------------
	end of file
----------------------------------------------------*/