#include "N79E81x.h"
#include"voice.h"



//��ʱ X ����
void delay_ms(unsigned int count)
{
	unsigned int i,j;
	for(i=0;i<count;i++)
	{
		for(j=0;j<500;j++);
	}
}




//��ʱ X ΢��
void delay_us(unsigned int count)
{
	unsigned int i,j;
	for(i=0;i<count;i++)
	{
		for(j=0;j<2;j++);
	}
}




//���Ƶ�ַ�η���
void SC_Speech(unsigned char cnt)
{
	unsigned char i;
//	SC_RST=1;
//	delay_ms(15); //DAC, ���� 32 ��Ϊ 15MS
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // ���������
		delay_us(350); // ��ʱ 100US
		SC_DATA=0; // ���������
		delay_us(350); // ��ʱ 100US
	}
}

void noVoice()
{
	P14=0;
	SC_DATA=0;
	SC_RST=0;
	delay_us(1000); // �ϵ縴λ��ֹ���ŷ���
	SC_RST=1;
	delay_us(1000);
	P14=1;
}


///************* ������ ****************/
//void main (void)
//{
//
//	P14=0;
//	SC_DATA=0;
//	SC_RST=1;
//	delay_us(1000); // �ϵ縴λ��ֹ���ŷ���
//	SC_RST=0;
//	delay_us(1000);
//	while(1)
//	{
//
//
//
//		P14=1;
//		SC_Speech(2); // �� 8 �������ź� , ���� 8 ������
//
//		
//		delay_ms(5000); // ��ʱ����ʱ�� 5 ��
//		P14=0;
//
//	}
//}