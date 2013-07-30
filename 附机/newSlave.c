#include"N79E81x.h"
#include<intrins.h>
#include"AD.h"
#include"UART.h"
#include"T0.h"
#include"voice.h"
#include"pwm.h"
#include"T1.h"

//����ͨ������

#define CmdStart 0x00 //��������
#define CmdStop 0x01  //�ػ�����

#define ComMode_1 0xc1 //ͨ��ģʽ1 
#define ComMode_2 0xc2 //ͨ��ģʽ2
#define ComMode_3 0xc3 //ͨ��ģʽ3
#define ComMode_4 0xc4 //̧��ָ��
#define ComMode_5 0xc5//����ָ��

#define Succeed 0xce  //ͨ�ųɹ�
#define Wrong 0xff    //ͨ��ʧ��

#define CmdHead 0xc8
#define CmdHead1 0x33 //����֡���ײ�1, 00110011,11
#define CmdHead2 0xcc //����֡���ײ�2,11001100,00
#define CmdHead3 0x3c //����֡���ײ�3,11000011,01
#define CmdHead4 0xcc //����֡���ײ�4,11001100,00

#define MyAddress 0xe0
#define MyAddress1 0x33 //������ַ1, 00110011,11
#define MyAddress2 0x3c //������ַ2, 11000011,01
#define MyAddress3 0xcc //������ַ3,11001100,00
#define MyAddress4 0xcc //������ַ4,11001100,00

//�����ķ��䲿�ֵĿ��ƶ˿�
//sbit PWMout=P3^5;//������ķ�������ڣ�����ʹ��PWM������
sbit ModeControl_1=P2^6;//�����ģʽ����,0��Ϊ1.5Mģʽ��1��Ϊ3Mģʽ

//���ջ�����
//sbit SwitchControl=P1^3;	//1�رս��ջ���0�������ջ�

//���ذ���
sbit Turn=P0^3;//����

//ģʽѡ�񰴼�
sbit ModeChange=P0^4;

//�����ƶ�
sbit Moto=P2^4;

//��ؿ��� 	AD��1��ͨ��Ϊ��صĵ�������
sbit BatteryControl=P1^2;

//���ջ�����
//sbit SwitchControl=P1^3;	//1��Ч��0�ر�
//�����pin18
//sbit tran_out=P1^0;
sbit receive_en=P1^3;		//���ջ�ʹ�ܣ�Ҫ����������

//���߷��������
sbit tran_en=P2^7;//��������أ�1��Ϊ���ˣ�0��Ϊ����

//����״̬���λ
bit TurnFlag=0;//0λ�ػ�״̬��1Ϊ����״̬

//ģʽѡ��λ��0����ģʽ1,1����ģʽ2
bit ModeFlag=0;

bit receiveFlag=0;//���յ����ݱ�־
bit commuFlag=0;//����ͨ�ű�־

bit alarmFlag2=0;//�Ƿ����������־
bit alarmFlag3=0;//�Ƿ����������־
bit alarmFlag4=0;	//̧�𱨾�
bit alarmFlag5=0;	//���ر���
unsigned int alarmCount2=0;//����2ѭ������
unsigned int alarmCount3=0;//����3ѭ������
unsigned int alarmCount4=0;//̧�𱨾�ѭ������
unsigned int alarmCount5=0;//���ر���ѭ������

bit threeFlag=0;//��·ѭ�����ر�־

bit power1Flag=0;
bit power2Flag=0;
bit power3Flag=0;
bit power4Flag=0;

unsigned char voiceFlag=0;//����ѭ������ 

unsigned char dataFirst=0;//���ڴ洢�ϴα�������

unsigned char count=0;//���ڽ��ղ��ֵļ�����
 
unsigned int time0Count_3=0;//��Ϊ����ÿ������������Ϣ����ʱ��

unsigned int lastAddr=0;//��һ�ν��յ��ı���ĵ�ַ
unsigned int TestFlag=0;//1��2��3�ֱ�Ϊÿ1S��ļ������ڴ��ڵĳɹ�ָ�����ִ�н�ȥ����Ĳ���
                		//�������3�ζ�û�й��㣬��˵�����ڳ���

//��Ϊ���պͷ��͵Ļ�����
unsigned char TxRxBuf[28]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//һ��ͷ�ֽڣ�һ����ַ�ֽڣ�һ�������ֽڣ����������ַ�ֽڣ���������
unsigned char myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};//��������ͨ�����ݵĻ�����

unsigned int time0Count_Clock=0;//�������ʱ��
unsigned int Check=0;//��ΪAD���ֵ
bit checkFlag=0;//��������־
 
unsigned char DataBetween=0;//��Ϊ�������ݵ��м����
unsigned char RecData=0;//���յ�������
unsigned char DataTime=0;//��Ϊ���յ����ݵ���λ��������
bit ComFlag=1;//�������ص�һ����־
unsigned char T1highcount=0;	   //��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����Datatime��0

//���ſ��ؿ��ƣ�1Ϊ�򿪹��ţ�0Ϊ�رչ���
sbit PAshutdown=P1^4;

//����һ������������ʾ�źŽ��պ󣬶೤ʱ��ʹ���ջ��򿪣�������SwitchControl�ĸߵ�ƽʱ�䡣
//unsigned int SwitchControlcount=0;
  
//��������
//void codeData(unsigned char *doData,unsigned char len);		//���� ,��ƽ1��Ϊ0011����ƽ0��Ϊ1100
//void transCode(unsigned char *doData,unsigned char len);//���룬�����յ������ݻ�ԭ
void ComMode_1_Data(void);//���ͱ���1

//��������
void StartAll(void);
//�ػ�����
void StopAll(void);

//t=1ʱ���ӳ�100us����
void Delay3(unsigned int t)
{
	unsigned int i,j;
	for(i=0;i<t;i++)		
	for(j=0;j<23;j++);
}

//init signal�����ͱ����ź�ǰ����ʼ�źţ����ڽ����ջ����Զ������
void initsignal()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<1;k1++)
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
//	Delay3(80);
}

void main(void)
{
//	unsigned int Check=0;//��ΪAD���ֵ	

	noVoice();
//	InitUART();
	InitT0();
	InitT1();
//	TI=0;
//	RI=0;

	Turn=1;		  //���߿���λ
	ModeChange=1;

	BatteryControl=0;//�����û�г����������Ϊ�͵�ƽ

	Moto=1;	//�ر����
//	SwitchControl=0;	 //������ջ��Ŀ��ƶ�Ϊ�ߵ�ƽ

//	ES=1;
	ET0=1;
	ET1=1;
	PT1=1;//��ʱ��1���ж����ȼ����
	EA=1;

	myPwm();	//�������

//	P1M1=0x02;
//	P1M2=0x00;
	P10=1;
//	P11=1;
	PAshutdown=0;		  //����ʱ�������Źر�
	
	Check=GetADCResult(6);//�ϵ�ʱ����ص������һ��
	
//����ֱ����������
//	SwitchControl=0;	 //������ջ��Ŀ��ƶ�Ϊ�ߵ�ƽ

	PAshutdown=1;
	SC_Speech(3);		 //�����Ѵ�
	Delay(100);
	PAshutdown=0;
																	   
//					StartAll();//�������������Ϳ���ָ��	
	commuFlag=1;//����ͨ��
	TurnFlag=1;

	alarmCount3=0;//�屨��������
	alarmFlag3=0;//�屨����־

	while(1)
	{
/*
		if(Turn==0)
		{
		 	Delay(30);
			if(Turn==0)
			{
//				while(Turn==0);
				if(TurnFlag==0)		 //˵���ǹػ�״̬,�򿪻�
				{
//					SwitchControl=0;	 //������ջ��Ŀ��ƶ�Ϊ�ߵ�ƽ

					PAshutdown=1;
					SC_Speech(3);		 //�����Ѵ�
					Delay(100);
					PAshutdown=0;
																					   
//					StartAll();//�������������Ϳ���ָ��	
					commuFlag=1;//����ͨ��
					TurnFlag=1;

					alarmCount3=0;//�屨��������
					alarmFlag3=0;//�屨����־
				}
				else
				{	 
					PAshutdown=1;
					SC_Speech(2);
					Delay(100);
					PAshutdown=0;
					Moto=1;//ֹͣ�����
//					StopAll();
  					commuFlag=0;//�ر�ͨ��
					TurnFlag=0;

					alarmCount3=0;//�屨��������
					alarmFlag3=0;//�屨����־

//					Check=GetADCResult(6);//��ص������
				}
			}
		}
*/
		if((alarmFlag3==1)&&(alarmCount3<3))//����3��ʼ��Ӧ�ı���
		{
			alarmCount3++;

			PAshutdown=1;
			SC_Speech(1);
			Delay(150);
			PAshutdown=0;
			
			Moto=0;//����
			Delay(10);
			Moto=1;
		}
//		if(alarmCount3>=2) //���������Ķ���	   
//		{
//			alarmCount3=0;//�屨��������
//			alarmFlag3=0;//�屨����־
//		}

		if((alarmFlag4==1)&&(alarmCount4<3))//̧��ʼ��Ӧ�ı���
		{
			alarmCount4++;

			PAshutdown=1;
			SC_Speech(5);
			Delay(180);
			
			Moto=0;//����
			Delay(10);
			Moto=1;
		}
//		if(alarmCount4>=1) //���������Ķ���	   
//		{
//			alarmCount4=0;//�屨��������
//			alarmFlag4=0;//�屨����־
//		}

		if((alarmFlag5==1)&&(alarmCount5<3))//���ؿ�ʼ��Ӧ�ı���
		{
			alarmCount5++;

			PAshutdown=1;
			SC_Speech(4);
			Delay(180);

			Moto=0;//����
			Delay(10);
			Moto=1;
		}
// 		if(alarmCount5>=1) //���������Ķ���	   
//		{
//			alarmCount5=0;//�屨��������
//			alarmFlag5=0;//�屨����־
//		}
/*
		if(Check>=0x377) //��ʾ��س����
		{
			BatteryControl=1;//��©ģʽ������Ϊ����̬	
		}
		else
		{
			BatteryControl=0;//�����û�г����������Ϊ�͵�ƽ
		}
*/
/*
		if((Check>=0x36f)&&(TurnFlag==1)&&(power1Flag==0))//���ñȽϵ�ѹ���˴�Ϊ4V,��4.2VΪ��׼
		{
			PAshutdown=1;
			SC_Speech(11);  //4V����������ʾ
			Delay(130);
			PAshutdown=0;

			power1Flag=1;
			power2Flag=0;
			power3Flag=0;
			power4Flag=0;
		}
	    else if((Check<0x36f)&&(Check>=0x365)&&(TurnFlag==1)&&(power2Flag==0))
		{
			PAshutdown=1;
			SC_Speech(10);  //3.8V����������ʾ
			Delay(130);
			PAshutdown=0;
	
			power2Flag=1;
			power1Flag=0;
			power3Flag=0;
			power4Flag=0;
		}
		else if((Check<0x365)&&(Check>=0x360)&&(TurnFlag==1)&&(power3Flag==0))
		{
			PAshutdown=1;
			SC_Speech(9);  //3.6V����������ʾ
			Delay(130);
			PAshutdown=0;

			power3Flag=1;
			power1Flag=0;
			power2Flag=0;
			power4Flag=0;
		}
		else if((Check<0x360)&&(TurnFlag==1)&&(power4Flag==0))
		{
			PAshutdown=1;
			SC_Speech(8);  //����3.6v����������ʾ
			Delay(130);
			PAshutdown=0;

			power4Flag=1;
			power1Flag=0;
			power2Flag=0;
			power3Flag=0;
		}
*/
	}
}

void timeT1() interrupt 3 //��ʱ��1�жϽ�������
{
//	unsigned int newAddr=0;
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
//				DataTime++;
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
		myTxRxData[count]=RecData;
		if(count==0&&myTxRxData[0]==CmdHead)
		{
			count=1;
		}
		else if(count==1&&myTxRxData[1]==MyAddress)
		{
			count=2;
		}
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
	}

	if(receiveFlag==1)	//˵�����յ������ݣ���ʼ����
	{
		receiveFlag=0;	//����ձ�־
		receive_en=0;			//�رս��ջ�
		switch(myTxRxData[2])//����ָ��
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

				Moto=0;//����
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

	if(time0Count_3>=60)//����ÿ1S����һ�ε����ݵ�ʱ���־
	{
		Check=GetADCResult(6);//��ص������

		if(commuFlag==1)//˵��������ͨ��
		{
			receive_en=0;		//�򿪽��ջ�
			ComMode_1_Data();//����ģʽ1�ź�
			receive_en=1;		//�򿪽��ջ�
			TestFlag++;
			
			if(TestFlag>=4)//˵���Ѿ�����300M�ˡ��ղ����κ��ź��ˣ�Ҫ������
			{
				TestFlag=5;
				//������Ӧ�������	
			}
		}
		time0Count_3=0;
	}
}

void StartAll()	//���Ϳ�ʼ�ź�
{
	unsigned char i,n;

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=CmdStart;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;
	
	initsignal();

	for(i=0;i<7;i++)
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
}

void StopAll() //����ֹͣ�ź�
{
	unsigned char i,n;

	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=CmdStop;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;

	initsignal();

	for(i=0;i<7;i++)
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
}

void ComMode_1_Data()//���ͱ���1
{
	unsigned char i,n;

	ModeControl_1=0;//30M���书��				
	tran_en=1;	//�����߷����
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_1;
	myTxRxData[3]=0x00;
	myTxRxData[4]=0x00;
	myTxRxData[5]=0x00;
	myTxRxData[6]=0x00;

	initsignal();

	for(i=0;i<7;i++)
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
	tran_en=0;
}