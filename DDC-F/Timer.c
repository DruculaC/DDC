/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "Other.h"

// ------ Public variable declarations -----------------------------
bit fell_alarm_flag = 0;			//���ر�����־
tByte fell_alarm_count = 0;		//���ر���ѭ������

bit raised_alarm_flag = 0;		//̧�𱨾���־
tByte raised_alarm_count = 0;	//̧�𱨾�ѭ������

bit stolen_alarm_flag = 0;		//����������־
tWord stolen_alarm_count = 0;	//��������ѭ������

bit transmit_comm1_flag = 0;		//ÿ��һ��ʱ�佫����1������main�����з������ݣ��Ͳ���Ӱ��timer0�ļ�ʱ�������������0.
bit comm_whole_control = 0;		//ͨ���ܿ��أ�0�ر�ͨ�ţ�1��ͨ��
bit battery_check = 0;				//��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
bit Host_battery_high_flag = 0;		//��1ʱ��ִ��һ��������ʾ����ʾ�������


// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;	//��ʱ��0��1�Ķ�ʱ����

tWord timer0_count = 0;			//��������timer0�Ĵ�����û���һ�ξͼ�1���൱�ڼ�ʱ
tByte leave_count = 0;				//ÿ��3s��1���������Ӧ����ȷ����������
tByte received_data_buffer[7] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	//�������ݻ���
bit receive_data_finished_flag = 0;		//������һ��������ɺ󣬴˱�־λ��1
tByte data_count = 0;				//�������ݻ����λ�����������ڼ�����������
tByte one_receive_byte = 0;		//�������ݵ�һ���ֽڣ���������丳ֵ��received_data_buffer���Ӧ���ֽ�
tByte one_receive_byte_count = 0;			//one_receive_byte��8λ���˼����������յ��ڼ�λ��ÿ�μ�����8��ʱ�����һ���ֽڽ�����ɡ�
bit receive_wire_flag = 1;		//����ͨ���ߵı�־λ��1�����ߵ�ƽ��0�����͵�ƽ��ÿ��timer1���ʱ���ж�P1.1һ�Ρ��Դ��������Ƿ�Ϊһ����ȷ���½���
tByte receive_HV_count = 0;		//��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����one_receive_byte_count��0
tByte receive_LV_count = 0;		//ÿ��timer1���ʱ�жϽ��������ΪLV���������1���Դ��������͵�ƽ��ʱ��

/*------------------------------------------------------------------
	timerT0()
	��ʱ��0ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
{
	TH0 = timer0_8H;
	TL0 = timer0_8L;

	timer0_count++;

	if(timer0_count >= 60)		//����ÿ1S����һ�ε����ݵ�ʱ���־
	{
		battery_check = 1;
		if(comm_whole_control == 1)		//˵��������ͨ��
		{
			leave_count++;
			transmit_comm1_flag = 1;
			if(leave_count >= 4)		//˵���Ѿ�����300M�ˡ��ղ����κ��ź��ˣ�Ҫ������
			{
				leave_count = 5;
			}
		}
		timer0_count = 0;
	}

	if(stolen_alarm_count >= 2)
	{
		stolen_alarm_count++;
		if(stolen_alarm_count == 1800)
		{
			stolen_alarm_count = 0;
			stolen_alarm_flag = 0;
		}
	}
}

/*------------------------------------------------------------------
	timerT1()
	��ʱ��1ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/
	
void timerT1() interrupt interrupt_timer_1_overflow 				//��ʱ��1�жϽ�������
{
	TH1 = timer1_8H;					//��װ��
	TL1 = timer1_8L;

	if(receive_wire == 0)							//�������Ϊ�ߵ�ƽ,�е͵�ƽ˵�����ź�
	{
		receive_LV_count++;
		receive_wire_flag=0;
		if(receive_LV_count==80)			//�͵�ƽ���������ʱ��	
		{
			receive_LV_count=0;
		}
	}
	else//Ϊ�ߵ�ƽ��
	{
		if(receive_wire_flag==0)//˵����һ���͵�ƽ
		{
			receive_wire_flag=1;
//			one_receive_byte<<=1;

			if((receive_LV_count>35)&&(receive_LV_count<=50))//�͵�ƽ������ʱ��С��3ms����Ϊ0
			{
				one_receive_byte<<=1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			if(receive_LV_count>50)//�͵�ƽ������ʱ�����4.5ms����Ϊ1
			{
				one_receive_byte<<=1;
				one_receive_byte |= 0x01;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			else
			{
				receive_HV_count++;	
			}

			receive_LV_count=0;
		}
		else
		{
			receive_HV_count++;
			if(receive_HV_count>=60)
			{
				one_receive_byte_count=0;
				receive_wire_flag=1;
				data_count=0;
			}		
		}
	}

	if(one_receive_byte_count==8)//˵��һ���ֽڵ������Ѿ�������ȫ
	{
		one_receive_byte_count=0;
		received_data_buffer[data_count]=one_receive_byte;
		if(data_count==0&&received_data_buffer[0]==CmdHead)
		{
			data_count=1;
		}
		else if(data_count==1&&received_data_buffer[1]==MyAddress)
		{
			data_count=2;
		}
		else if(data_count==2)
		{
			receive_data_finished_flag=1;
			data_count=0;
		}
		else 
		{
			data_count=0;
		}
	}

	if(receive_data_finished_flag==1)	//˵�����յ������ݣ���ʼ����
	{
		receive_data_finished_flag=0;	//����ձ�־
		Receiver_EN=0;			//�رս��ջ�
		switch(received_data_buffer[2])//����ָ��
		{
			case ComMode_1://���յ������������͹����ı���1���źţ�˵��������3M�ڣ���������
			{	
				leave_count=0;//����������峬ʱ��־

				stolen_alarm_count=0;//�屨��������
				stolen_alarm_flag=0;//�屨����־

				raised_alarm_count=0;//�屨��������
				raised_alarm_flag=0;//�屨����־

				fell_alarm_count=0;//�屨��������
				fell_alarm_flag=0;//�屨����־

				Moto_Vibration();
			}
			break;
		
			case ComMode_3:
			{
				leave_count=0;//�峬ʱ��־				
				stolen_alarm_flag=1;

				raised_alarm_count=0;//�屨��������
				raised_alarm_flag=0;//�屨����־
				fell_alarm_count=0;//�屨��������
				fell_alarm_flag=0;//�屨����־

				Moto_Vibration();
			}
			break;
		
			case ComMode_4://����̧���ź�ʹ��
			{
				leave_count=0;//�峬ʱ��־	
				raised_alarm_flag=1;//̧�𱨾�

				stolen_alarm_count=0;//�屨��������
				stolen_alarm_flag=0;//�屨����־
				fell_alarm_count=0;//�屨��������
				fell_alarm_flag=0;//�屨����־

				Moto_Vibration();
			}
			break;

			case ComMode_5://���������ź�ʹ��
			{
				leave_count=0;//�峬ʱ��־
				fell_alarm_flag=1;	//���ر���

				stolen_alarm_count=0;//�屨��������
				stolen_alarm_flag=0;//�屨����־
				raised_alarm_count=0;//�屨��������
				raised_alarm_flag=0;//�屨����־

				Moto_Vibration();
			}
			break;

			case ComMode_6:
			{
				Host_battery_high_flag=1;
			}
			break;
		}
	}
}

/*--------------------------------------------------
	InitTimer()
	
	��ʼ����ʱ��T0��T1
---------------------------------------------------*/

void InitTimer(const tByte Tick_ms_T0, Tick_us_T1)
	{
	tLong Inc_T0, Inc_T1;
	tWord timer0_16, timer1_16;
	
	//����Timer0�ļĴ���ֵ
	Inc_T0 = (tLong)Tick_ms_T0 * (OSC_FREQ/1000) / (tLong)OSC_PER_INST;
	timer0_16 = (tWord) (65536UL - Inc_T0);
	timer0_8H = (tByte) (timer0_16 / 256);
	timer0_8L = (tByte) (timer0_16 % 256);
	
	//����Timer1�ļĴ���ֵ
	Inc_T1 = (tLong)Tick_us_T1 * (OSC_FREQ/1000000) / (tLong)OSC_PER_INST;
	timer1_16 = (tWord) (65536UL - Inc_T1);
	timer1_8H = (tByte) (timer1_16 / 256);
	timer1_8L = (tByte) (timer1_16 % 256);
	
	TMOD = 0x11;
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	TH1 = timer1_8H;
	TL1 = timer1_8L;

	ET0 = 1;
	TR0 = 1;
	ET1 = 1;
	TR1 = 1;
	PT1 = 1;			
	EA = 1;
	}

/*---------------------------------------------------
	end of file
----------------------------------------------------*/