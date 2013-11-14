/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "voice.h"

// ------ Public variable declarations -----------------------------
bit stolen_alarm_flag = 0;		//��������������־��1��ʱ���ʾ����		
bit host_stolen_speech_EN = 0;		//������������ʹ�ܣ�1��ʱ����main��������ʾ
bit battery_check=0;		//��1ʱ��ִ��һ�ε���ת����ִ����󣬽�����0
bit position_sensor_EN=0;  		//λ�ô�������������̧�𴫸����ܿ��أ�1��ʱ�򣬼��������������
bit slave_away_speech_EN=0;      //�жϸ����뿪��������ʾ����main�������
bit magnet_CW_EN = 0;					//�����˳ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
bit magnet_ACW_EN=0;						//�������ʱ��ת��ʹ�ܣ�ת��һ�κ�λΪ0
bit comm_whole_control=0;				//ͨ���ܿ��أ�0�ر�ͨ�ţ�1��ͨ��
tWord host_stolen_speech_count=0;   //�ж�����������������ʾ�Ĵ���
bit slave_nearby_speech_EN=0;       //�жϸ���������������ʾ����main�������

// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;	//��ʱ��0��1�Ķ�ʱ����

tByte host_touch_speech_count=0;
tByte sensor_trigger_count=0;	//��������������
tByte host_touched_flag=0;			//��������������1�����к���Ĵ������ж�
tWord sensor_2ndstage_time=0;		//��������һ�׶κ�����ʱ��ļ���
tByte sensor_1ststage_count=0;	//��������һ�׶��жϵ͵�ƽ�ļ���
tByte raised_alarm_count = 0;    //������̧����򸽻����������źŵĴ���
bit raised_flag=0;					//�ж�������̧�����1
tByte fell_alarm_count=0;        //�������غ��򸽻����������źŵĴ���
bit fell_flag=0;						//�ж��������º���1
tWord timer0_count=0;				//timer0ÿ��������1����ʾ��timer0��ʱ
tByte slave_away=0;					//ģʽѡ��λ��1����ģʽ1,2����ģʽ2,3��Ϊģʽ3
tByte leave_count=0;					//�����뿪ʱ�ļ�����ֻҪ����3sӦ����ȷ������0	
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//�������ݻ���
bit receive_data_finished_flag = 0;		//������һ��������ɺ󣬴˱�־λ��1
tByte data_count = 0;				//�������ݻ����λ�����������ڼ�����������
tByte one_receive_byte = 0;		//�������ݵ�һ���ֽڣ���������丳ֵ��received_data_buffer���Ӧ���ֽ�
tByte one_receive_byte_count = 0;			//one_receive_byte��8λ���˼����������յ��ڼ�λ��ÿ�μ�����8��ʱ�����һ���ֽڽ�����ɡ�
bit receive_wire_flag = 1;		//����ͨ���ߵı�־λ��1�����ߵ�ƽ��0�����͵�ƽ��ÿ��timer1���ʱ���ж�P1.1һ�Ρ��Դ��������Ƿ�Ϊһ����ȷ���½���
tByte receive_HV_count = 0;		//��ʱ��T1��û���źŵ�����ʱ�򣬶Ըߵ�ƽ������һ������ĳ��ֵ����one_receive_byte_count��0
tByte receive_LV_count = 0;		//ÿ��timer1���ʱ�жϽ��������ΪLV���������1���Դ��������͵�ƽ��ʱ��
tByte fell_wire_time=0;          //���ؼ���ߣ����͵�ƽ��ʱ��
tByte raise_wire_time=0;			//̧�����ߣ����͵�ƽ��ʱ��
tByte sensor_2ndstage_LV_time=0; 	//����������ڶ��׶μ��ʱ������͵�ƽ��ʱ��


/*------------------------------------------------------------------
	timerT0()
	��ʱ��0ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow	//��Ϊ����ϵͳ�Լ���ʱ��
{
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	timer0_count++;

	if(timer0_count>=2000)//����ÿ3s����һ�ε����ݵ�ʱ���־
	{
		if(comm_whole_control==1)//˵��������ͨ��
		{
			leave_count++;

			if(leave_count>=4&&slave_away==0)//˵��û�н��յ������Ѿ���3���ˣ������Ѿ�����3M�����ھ�Ҫ�Ӵ��ʣ��л���ģʽ2,30M�ٿ��ܲ��ܽ��յ�����
			{
				leave_count=5;

				magnet_CW_EN=1;		 	//���������
				slave_away_speech_EN=1;		//�������뿪����
				
				position_sensor_EN=1;		//�������ء�̧���־
				slave_away=1;

				sensor_trigger_count=0;
				sensor_1ststage_count=0;
					
 			}
 		}
		timer0_count=0;
		
		battery_check=1;		
		
		if((fell_flag==1)&&(fell_alarm_count<5))  //���غ�����Ӧ�Ķ���
		{
			ComMode_5_Data(); //�򸽻����ͱ���3
			fell_alarm_count++;
		}
		if((raised_flag==1)&&(raised_alarm_count<5))		//̧�������Ӧ����
		{
//			sendcomm4=1;
			ComMode_4_Data(); //�򸽻����ͱ���3
			raised_alarm_count++;
		}
	}

	if(sensor_EN==1)	//������ᴫ�����Ƿ�򿪣����һ�û�б���
	{
		if((sensor_detect==0)&&(stolen_alarm_flag==0))
		{
			sensor_1ststage_count++;
			if(sensor_1ststage_count>=8)				 //ÿ1ms���һ�θߵ�ƽ�����������6ms�ĸ߶�ƽ��˵����������һ��
			{
				sensor_1ststage_count=0;
				sensor_trigger_count++;
				host_touch_speech_count=0;
				host_touched_flag=1;
			}
		}
		else
		{
			sensor_1ststage_count=0;
		}
		
		if(host_touched_flag==1)
		{
			sensor_2ndstage_time++;
			if(sensor_2ndstage_time>=6000)
			{
				sensor_trigger_count=0;
				sensor_1ststage_count=0;
				sensor_2ndstage_time=0;
				host_touched_flag=0;
				host_touch_speech_count=0;
			}
		}
	}

	if(sensor_trigger_count==1)
	{
		if((host_touch_speech_count<1)&&(host_stolen_speech_EN!=1))
		{
			if((fell_sensor_detect==1)&&(raised_sensor_detect==1))
			{
				host_touch_speech();
				host_touch_speech_count++;
			}
		}
	}
	else
	{
		if(sensor_2ndstage_time>=3000)
		{
			if(sensor_detect==0)
			{
				sensor_2ndstage_LV_time++;
				if(sensor_2ndstage_LV_time>=6)	
				{
					host_stolen_speech_EN=1;
					host_stolen_speech_count=0;
					stolen_alarm_flag=1;
					sensor_2ndstage_LV_time=0;	
				}
			}
			else
			{
				sensor_2ndstage_LV_time=0;
			}
		}						 
	}

//	��⵹�غ�̧����Ĵ���
	if(position_sensor_EN==1)//������̧�𵹵ؼ��
	{
		if(raised_sensor_detect==0)//˵����̧���źŲ����ǵ�һ�Σ���ʼ��ʱ
		{
			raise_wire_time++;
			if(raise_wire_time==10)//˵���Ѿ�����0.5S
			{
				raised_flag=1;//��̧���־
				fell_flag=0;
			}		
		}
		else
		{
			raised_flag=0;
			raised_alarm_count=0;
			raise_wire_time=0;
		}

		if(fell_sensor_detect==0)
		{
			fell_wire_time++;
			if(fell_wire_time==10)//˵���Ѿ�����0.5S
			{
				fell_flag=1;//��̧���־
				raised_flag=0;
			}		
		}
		else
		{
			fell_flag=0;
			fell_alarm_count=0;
			fell_wire_time=0;
		}
	}
}


/*------------------------------------------------------------------
	timerT1()
	��ʱ��1ÿ�������ִ�еĲ���
--------------------------------------------------------------------*/

void timerT1() interrupt interrupt_timer_1_overflow
{

	TH1 = timer1_8H;					//��װ��
	TL1 = timer1_8L;
	
	if(P11==0)//�������Ϊ�ߵ�ƽ,�е͵�ƽ˵�����ź�
	{
		receive_LV_count++;
		receive_wire_flag=0;
		if(receive_LV_count==80)//�͵�ƽ���������ʱ��	
		{
			receive_LV_count=0;
		}
	}
	else//Ϊ�ߵ�ƽ��
	{
		if(receive_wire_flag==0)//˵����һ���͵�ƽ
		{
			receive_wire_flag=1;

			if((receive_LV_count>35)&&(receive_LV_count<=50))	//�͵�ƽ������ʱ��С��10ms����Ϊ0
			{
				one_receive_byte<<=1;
				one_receive_byte &= 0xfe;
				one_receive_byte_count++;
				receive_HV_count=0;
			}
			else if((receive_LV_count>50))//�͵�ƽ������ʱ�����4.5ms����Ϊ1
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

	if(receive_data_finished_flag==1)
	{
		receive_data_finished_flag=0;
		switch(received_data_buffer[2]) 		//������֡���������д���
		{
			case ComMode_1:  		//�������͹�����ֻ��ģʽ1��˵�������������ģ����ݲ���Ϊ����ĵ�һ�͵ڶ����ֽڣ�Ϊ������ڵ��������Ŀ�ʼ�ֽڵ��Ǹ���ַ��Ȼ���������֡�������������ݷ��ͳ�ȥ
			{
				stolen_alarm_flag=0;
				
				ComMode_1_Data(); 	//�򸽻����ͱ���3

				slave_nearby_speech_EN=1;		//����1��һ������


				sensor_EN=0;	//���ᴫ����
				position_sensor_EN=0; 		//�ص��ء�̧����
				fell_flag=0;
				raised_flag=0;
				magnet_ACW_EN=1;			//�򿪵����
				
				sensor_trigger_count=0;
				sensor_1ststage_count=0;
				sensor_2ndstage_time=0;
				host_touched_flag=0;
				host_stolen_speech_EN=0;
				host_stolen_speech_count=0;

				leave_count=0;	
				if(slave_away==1)
				{
					slave_away=0;
				}
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