/*---------------------------------------------------
	communication.c (v1.00)
	
	通信程序
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "communication.h"
#include "Delay.h"

/*-------------------------------------------------*/
tByte myTxRxData[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};

extern tByte receive_LV_count;		//每次timer1溢出时判断接收线如果为LV，则计数加1，以此来表明低电平的时间
extern bit receive_wire_flag;		//接收通信线的标志位，1表明高电平，0表明低电平，每次timer1溢出时，判断P1.1一次。以此来表明是否为一次正确的下降沿
extern tByte one_receive_byte;		//接收数据的一个字节，接收完后将其赋值给received_data_buffer相对应的字节
extern tByte one_receive_byte_count;			//one_receive_byte有8位，此计数表明接收到第几位，每次计数到8的时候表明一个字节接收完成。
extern tByte receive_HV_count;		//定时器T1在没有信号到来的时候，对高电平计数，一旦超过某个值，则将one_receive_byte_count清0
extern tByte data_count;				//接收数据缓存的位数，即表明第几个缓存数据
extern tByte received_data_buffer[7];		//接收数据缓存
extern bit receive_data_finished_flag;		//接收这一串数据完成后，此标志位置1

/*----------------------------------------------------
	initsignal()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal2()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal2()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay3(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*----------------------------------------------------
	initsignal3()
	
	初始化信号程序，接收机在接接收信号的时候，需要有
	一段代码来使接收机打开，而这一段程序有可能无法识别
	出来，而是相当于废代码，只占用通信时间。
----------------------------------------------------*/

void initsignal3()
{
	unsigned char k,k1;
	unsigned char mystartbuffer=0xaa;
	for(k1=0;k1<3;k1++)
	{
		for(k=0;k<8;k++)
		{
			if((mystartbuffer&0x80)==0x80)//为1
			{
				P10=0;
				Delay3(80);	//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else
			{
				P10=0;
				Delay3(80);	//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;
			mystartbuffer<<=1;
			Delay3(150);//延时要大于2ms
		}
		mystartbuffer=0xaa;
		Delay3(80);
	}
	P10=1;
}

/*------------------------------------------------------------------------
	

/*--------------------------------------------------
	ComMode_1_Data()
	
	主机接收到编码1信号后，会反馈一个编码1信号给附机
	以表示主机在附机附近。
---------------------------------------------------*/

void ComMode_1_Data()	
{
	unsigned char i,n;
	receiver_EN = 1;
	transmiter_power = 1;				
	transmiter_EN = 0;
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
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN = 1;
	receiver_EN = 0;
	transmiter_power = 1;				
}

/*--------------------------------------------------
	ComMode_2_Data()
	when battery is stolen, transmit this code to slave. 
---------------------------------------------------*/

void ComMode_2_Data()	
{
	unsigned char i,n;
	receiver_EN = 1;
	transmiter_power = 0;				
	transmiter_EN = 0;
	myTxRxData[0]=CmdHead;
	myTxRxData[1]=MyAddress;
	myTxRxData[2]=ComMode_2;
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
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN = 1;
	transmiter_power = 1;				
	receiver_EN = 0;
}

/*----------------------------------------------------------
	ComMode_3_Data()
	
	被盗报警信号，让附机语音提示“电动车被盗“
----------------------------------------------------------*/

void ComMode_3_Data()
{
//	unsigned int j;
	unsigned char i,n;
	receiver_EN = 1;
	transmiter_power = 0;
	transmiter_EN = 0;
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
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN = 1;
	transmiter_power = 1;
	receiver_EN = 0;
}

/*--------------------------------------------------------
	ComMode_4_Data()
	
	发信号给附机，让其语音提示“电动车被抬起”
---------------------------------------------------------*/

void ComMode_4_Data()//发送抬起编码
{
	unsigned char i,n;
	receiver_EN = 1;
	transmiter_power = 0;
	transmiter_EN = 0;
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
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN = 1;
	transmiter_power = 1;
	receiver_EN = 0;
}

/*-----------------------------------------------------------
	ComMode_5_Data()
	
	发信号给附机，让其语音提示“电动车倒地了”
------------------------------------------------------------*/

void ComMode_5_Data()//发送倒地编码
{
	unsigned char i,n;
	receiver_EN = 1;
	transmiter_power = 0;//切换为300M发射
	transmiter_EN = 0;      //打开无线发射机
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
			if((myTxRxData[i]&0x80)==0x80)//为1
			{
				P10=0;
				Delay4(120);//延时4.5ms以上，由于定时器占用问题，只能用这种延时来实现
			}
			else//为0的情况
			{
				P10=0;
				Delay4(80);//延时2ms，由于定时器占用问题，只能用这种延时来实现
			}
			P10=1;//常态为高电平
			myTxRxData[i]<<=1;
			Delay4(50);//延时要大于2ms
		}
	}
	transmiter_EN = 1;
	transmiter_power = 1;
	receiver_EN = 0;
}

/*-----------------------------------------------------------------------------
	receive_byte()
	receive a byte program
-----------------------------------------------------------------------------*/
void receive_byte(void)
	{
	// P11 constantly HV, if detected a LV, judge it.
	if(P11 == 0)
		{
		// count the LV time, if more than 12ms, reset it.
		if(++receive_LV_count == 120)
			{
			receive_LV_count = 0;
			}
		receive_wire_flag = 0;
		}
	// if P11 return to HV
	else
		{
		// and already have LV before, so we think it maybe a bit signal
		if(receive_wire_flag == 0)
			{
			// set the flag, to judge a bit only one time
			receive_wire_flag = 1;

			// judge the LV time, if 3.5ms < time < 8ms, we think it is a "0". time<3.5ms means a noise.
			if((receive_LV_count > 35)&&(receive_LV_count <= 80))	
				{
				// save "0" to one byte
				one_receive_byte <<= 1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count = 0;
				}
			// time > 8ms, means a "1"
			else if((receive_LV_count > 80))
				{
				// save "1" to one byte
				one_receive_byte <<= 1;
				one_receive_byte |= 0x01;
				one_receive_byte_count++;
				receive_HV_count = 0;
				}
			
			else
				{
				// increase the count for HV
				receive_HV_count++;	
				}
         // reset LV count
			receive_LV_count = 0;
			}
		else
			{
			// judge whether HV count > 6ms, if yes, means a restart
			if(++receive_HV_count >= 60)
				{
				one_receive_byte_count = 0;
				receive_wire_flag = 1;
				data_count = 0;
				}		
			}
		}
	}

/*-----------------------------------------------------------------------------
	receive_word()
	receive a word program
-----------------------------------------------------------------------------*/
void receive_word(void)
	{
	// one byte have 8 bit, once receive a bit, the count increase, once it is 8, it means a byte received.
	if(one_receive_byte_count == 8)
		{
		one_receive_byte_count = 0;
		// assign one byte to buffer[i] 
		received_data_buffer[data_count] = one_receive_byte;
		
		// judge whether buffer[0] is CmdHead
		if((data_count == 0) && (received_data_buffer[0] == CmdHead))
			{
			data_count = 1;
			}
		// judge whether buffer[1] is MyAddress
		else if((data_count == 1) && (received_data_buffer[1] == MyAddress))
			{
			data_count = 2;
			}
		else if(data_count == 2)
			{
			receive_data_finished_flag = 1;
			data_count = 0;
			}
		else 
			{
			data_count = 0;
			}
		}
	}
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/