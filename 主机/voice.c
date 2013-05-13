#include "N79E81x.h"
#include"voice.h"



//延时 X 毫秒
void delay_ms(unsigned int count)
{
	unsigned int i,j;
	for(i=0;i<count;i++)
	{
		for(j=0;j<500;j++);
	}
}




//延时 X 微秒
void delay_us(unsigned int count)
{
	unsigned int i,j;
	for(i=0;i<count;i++)
	{
		for(j=0;j<2;j++);
	}
}




//控制地址段放音
void SC_Speech(unsigned char cnt)
{
	unsigned char i;
//	SC_RST=1;
//	delay_ms(15); //DAC, 大于 32 段为 15MS
	SC_RST=0;
	delay_ms(40);
	SC_RST=1;
	delay_ms(40);
	for(i=0;i < cnt;i++)
	{
		SC_DATA=1; // 数据脉冲高
		delay_us(350); // 延时 100US
		SC_DATA=0; // 数据脉冲低
		delay_us(350); // 延时 100US
	}
}

void noVoice()
{
	P14=0;
	SC_DATA=0;
	SC_RST=0;
	delay_us(1000); // 上电复位防止干扰发声
	SC_RST=1;
	delay_us(1000);
	P14=1;
}


///************* 主程序 ****************/
//void main (void)
//{
//
//	P14=0;
//	SC_DATA=0;
//	SC_RST=1;
//	delay_us(1000); // 上电复位防止干扰发声
//	SC_RST=0;
//	delay_us(1000);
//	while(1)
//	{
//
//
//
//		P14=1;
//		SC_Speech(2); // 发 8 个触发信号 , 发第 8 段声音
//
//		
//		delay_ms(5000); // 延时放音时间 5 秒
//		P14=0;
//
//	}
//}