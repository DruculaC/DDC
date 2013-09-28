/*----------------------------------------------------
	port.h (v1.00)

	'port header' (see chap 5) for DDC-Z
----------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#include "main.h"

//���ñ������İ�ť�����綯��Կ��ת���󣬱�һ�ε���
//�����Ǵ�0��1��������1��0����������ʾ��һ����
sbit det_battery=P2^4;			

//sbit PWMout=P0^1;//������ķ�������ڣ�ʹ������PWM
sbit ModeControl_1=P2^6;//�����ģʽ����,0��Ϊ30Mģʽ��1��Ϊ300Mģʽ
sbit tran_en=P2^7;//��������أ�1��Ϊ���ˣ�0��Ϊ����

sbit ReceWave=P0^7;//���ᴫ���������
sbit SensorControl=P2^5;//���ᴫ�������ƶ�

//���ó��ָʾ
//sbit det_charge=P0^2;

//�����,  ƽʱֵΪ00��������10��������01��Ȼ�󶼻ָ���00
sbit MagentControl_1=P2^2;
sbit MagentControl_2=P2^3;

sbit upSignal=P0^4;//̧���ź�
sbit downSignal=P0^3;//�����ź�

//��ؿ��� 	AD��1��ͨ��Ϊ��صĵ�������
sbit BatteryControl=P1^2;

//���ſ��ؿ��ƣ�1Ϊ�򿪹��ţ�0Ϊ�رչ���
sbit PAshutdown=P1^4;

sbit SC_RST=P2^0; //P2.0 �����帴λ��
sbit SC_DATA=P2^1; //P2.1 ���������ݽ�

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/