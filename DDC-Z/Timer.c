/*---------------------------------------------------
	Timer.c (v1.00)
	
	Timer functions
---------------------------------------------------*/	

#include "main.h"
#include "port.h"

#include "Timer.h"
#include "communication.h"
#include "voice.h"
#include "Delay.h"
#include "Battery.h"
#include "AD.h"
#include "Other.h"
#include "operation.h"

// ------ Public variable declarations -----------------------------
bit stolen_alarm_flag = 0;					// when host been touch 3 times, this flag 1 before alarm voice present, not to detect sensor for 1st voice alarm.
bit host_stolen_speech_EN = 0;			// when host been touch 3 times, enable 3rd alarm voice.
bit battery_check=0;							// 2ÖÃ1Ê±£¬Ö´ÐÐÒ»´ÎµçÁ¿×ª»»£¬Ö´ÐÐÍêºó£¬½«ÆäÖÃ0
bit position_sensor_EN=0;  		//Î»ÖÃ´«¸ÐÆ÷£¬¼´µ¹µØÌ§Æð´«¸ÐÆ÷×Ü¿ª¹Ø£¬1µÄÊ±ºò£¬¼ì²âÕâÁ½¸ö´«¸ÐÆ÷
bit slave_away_speech_EN=0;      //ÅÐ¶Ï¸½»úÀë¿ªºó£¬ÓïÒôÌáÊ¾£¬ÔÚmainÀïÃæ²Ù×÷
bit magnet_CW_EN = 0;					//µç´ÅÌúË³Ê±Õë×ª¶¯Ê¹ÄÜ£¬×ª¶¯Ò»´Îºó¸´Î»Îª0
bit magnet_ACW_EN=0;						//µç´ÅÌúÄæÊ±Õë×ª¶¯Ê¹ÄÜ£¬×ª¶¯Ò»´Îºó¸´Î»Îª0
bit comm_whole_control=0;				//Í¨ÐÅ×Ü¿ª¹Ø£¬0¹Ø±ÕÍ¨ÐÅ£¬1´ò¿ªÍ¨ÐÅ
tWord host_stolen_speech_count=0;   //ÅÐ¶ÏÖ÷»ú±»µÁºó£¬ÓïÒôÌáÊ¾µÄ´ÎÊý
bit slave_nearby_speech_EN=0;       //ÅÐ¶Ï¸½½ü¿¿½üºó£¬ÓïÒôÌáÊ¾£¬ÔÚmainÀïÃæ²Ù×÷
bit host_stolen_alarm1_EN = 0;      //ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒôÊ¹ÄÜ
bit host_stolen_alarm2_EN = 0;      //ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶þ¶ÎÓïÒôÊ¹ÄÜ
tByte host_stolen_alarm1_count = 0;		//ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚÒ»¶ÎÓïÒô´ÎÊý
tByte host_stolen_alarm2_count = 0;		//ÅÐ¶ÏÎª±»µÁ±¨¾¯ºóµÄµÚ¶þ¶ÎÓïÒô´ÎÊý
bit fell_flag=0;						//ÅÐ¶ÏÖ÷»úµ¹ÏÂºó£¬ÖÃ1
bit slave_away = 1;					// flag for slave, 1 means away, 0 means nearby
tByte leave_count=0;					// increment every 2s, reset for every success communication
bit raised_flag=0;					//ÅÐ¶ÏÖ÷»ú±»Ì§Æðºó£¬ÖÃ1
tByte sensor_2ndstage_LV_time=0; 	//´«¸ÐÆ÷½øÈëµÚ¶þ½×¶Î¼ì²âÊ±£¬¼ÆËãµÍµçÆ½µÄÊ±¼ä
tByte sensor_2ndstage_count = 0;		//´«¸ÐÆ÷½øÈëµÚ¶þ½×¶Î¼ì²âÊ±£¬¼ÆËãµÍµçÆ½µÄÊ±¼ä
tByte host_2ndtouch_speech_count = 0;		//Ö÷»ú±»µÚ¶þ´Î´¥Åöºó£¬ÓïÒôÌáÊ¾µÄ´ÎÊý
tWord sensor_3rdstage_time = 0;			//´«¸ÐÆ÷½øÈëµÚÈý½×¶ÎµÄÊ±¼ä£¬
tByte sensor_3rdstage_effcount = 0;		//´«¸ÐÆ÷½øÈëµÚÈý½×¶Îºó£¬ÓÐÐ§´¥Åö´ÎÊýµÄ¼ÆÊý
tByte sensor_3rdstage_count = 0;			//´«¸ÐÆ÷½øÈëµÚÈý½×¶Îºó£¬µÍµçÆ½µÄ¼ÆÊý
tWord sensor_3rdstage_interval = 0;		//´«¸ÐÆ÷ÔÚµÚÈý½×¶ÎÖÐ£¬Ã¿´ÎÓÐÐ§µÍµçÆ½¼ÆÊýÖ®¼äµÄÊ±¼ä¼ä¸ô¡£ÔÚÕâÆÚ¼äµÄµÍµçÆ½²»ÈÏÎªÓÐÐ§¡£
bit host_touch_speech_EN = 0;				//µÚÒ»´Î´¥ÅöºóÓïÒôÊ¹ÄÜ
bit host_2ndtouch_speech_EN = 0;			//µÚ¶þ´Î´¥ÅöºóÓïÒôÊ¹ÄÜ
bit raised_fell_flag = 0;					//µ¹µØ»òÕßÌ§Æð´¥·¢ºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte sensor_trigger_count=0;		//´«¸ÐÆ÷´¥·¢¼ÆÊý
tWord sensor_2ndstage_time=0;		//´«¸ÐÆ÷½øÈëµÚ¶þ½×¶ÎºóÁ÷ÊÅÊ±¼äµÄ¼ÆÊý
tByte sensor_1ststage_count=0;	//´«¸ÐÆ÷µÚÒ»½×¶ÎÅÐ¶ÏµÍµçÆ½µÄ¼ÆÊý
tByte nearby_away_interval = 0;		//¸½»úÀë¿ªºÍ¿¿½üÊ±£¬ÓïÒôÌáÊ¾ºÍ¿ª¹ØËøµÄÊ±¼ä¼ä¸ô
bit magnet_ACW_flag=0;

// ------ Private variable definitions -----------------------------
tByte timer0_8H, timer0_8L, timer1_8H, timer1_8L;		// register value of timer0 and timer1, caculated from InitTimer()

tByte host_touch_speech_count=0;
tByte host_touched_flag=0;			//Ö÷»ú±»´¥ÅöºóÖÃ1£¬½øÐÐºóÃæµÄ´«¸ÐÆ÷ÅÐ¶Ï
tByte raised_alarm_count = 0;    //Ö÷»ú±»Ì§Æðºó£¬Ïò¸½»ú·¢³ö±¨¾¯ÐÅºÅµÄ´ÎÊý
tByte fell_alarm_count=0;        //Ö÷»úµ¹µØºó£¬Ïò¸½»ú·¢³ö±¨¾¯ÐÅºÅµÄ´ÎÊý
tWord timer0_count=0;		// counter for timer0, increment every ticket 			
tByte received_data_buffer[7]={0x00,0x00,0x00,0x00,0x00,0x00,0x00};		//½ÓÊÕÊý¾Ý»º´æ
bit receive_data_finished_flag = 0;		//½ÓÊÕÕâÒ»´®Êý¾ÝÍê³Éºó£¬´Ë±êÖ¾Î»ÖÃ1
tByte data_count = 0;				//½ÓÊÕÊý¾Ý»º´æµÄÎ»Êý£¬¼´±íÃ÷µÚ¼¸¸ö»º´æÊý¾Ý
tByte one_receive_byte = 0;		//½ÓÊÕÊý¾ÝµÄÒ»¸ö×Ö½Ú£¬½ÓÊÕÍêºó½«Æä¸³Öµ¸øreceived_data_bufferÏà¶ÔÓ¦µÄ×Ö½Ú
tByte one_receive_byte_count = 0;			//one_receive_byteÓÐ8Î»£¬´Ë¼ÆÊý±íÃ÷½ÓÊÕµ½µÚ¼¸Î»£¬Ã¿´Î¼ÆÊýµ½8µÄÊ±ºò±íÃ÷Ò»¸ö×Ö½Ú½ÓÊÕÍê³É¡£
bit receive_wire_flag = 1;		//½ÓÊÕÍ¨ÐÅÏßµÄ±êÖ¾Î»£¬1±íÃ÷¸ßµçÆ½£¬0±íÃ÷µÍµçÆ½£¬Ã¿´Îtimer1Òç³öÊ±£¬ÅÐ¶ÏP1.1Ò»´Î¡£ÒÔ´ËÀ´±íÃ÷ÊÇ·ñÎªÒ»´ÎÕýÈ·µÄÏÂ½µÑØ
tByte receive_HV_count = 0;		//¶¨Ê±Æ÷T1ÔÚÃ»ÓÐÐÅºÅµ½À´µÄÊ±ºò£¬¶Ô¸ßµçÆ½¼ÆÊý£¬Ò»µ©³¬¹ýÄ³¸öÖµ£¬Ôò½«one_receive_byte_countÇå0
tByte receive_LV_count = 0;		//Ã¿´Îtimer1Òç³öÊ±ÅÐ¶Ï½ÓÊÕÏßÈç¹ûÎªLV£¬Ôò¼ÆÊý¼Ó1£¬ÒÔ´ËÀ´±íÃ÷µÍµçÆ½µÄÊ±¼ä
tByte fell_wire_time=0;          //µ¹µØ¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tByte raise_wire_time=0;			//Ì§Æð¼ì²âÏß£¬¼ì²âµÍµçÆ½µÄÊ±¼ä
tWord raised_fell_number = 0;				//µ¹µØ»òÕßÌ§Æð³ö·¢ºó£¬¼ÆÊý£¬µ½´ïÒ»¶¨ÊýÖµºó£¬½«ÆäÓë±êÖ¾Î»Ò»ÆðÇåÁã¡£
bit raised_fell_once_flag = 0;			//raised_fell_flagÊÇ·ñÔø¾­±êÖ¾¹ý£¬Èç¹û±êÖ¾¹ýÔòÖÃ1.È»ºóÖ÷»ú±»»Ö¸´µ¹µØ»òÕß»Ö¸´Ì§ÆðÊ±£¬´Ë±êÖ¾Î»¸´Î»¡£
tByte key_rotated_on_flag=0;			//µç¶¯³µ¿ªÆô¹Ø±Õ±êÖ¾Î»£¬1±íÊ¾µç¶¯³µ¿ªÆôÁË£¬0±íÊ¾µç¶¯³µ¹Ø±ÕÁË
tByte slave_nearby_speech_count=0;
tByte slave_away_speech_count=0;
tWord ADC_check_result = 0;		//×÷ÎªAD¼ì²âÖµ
bit slave_nearby_EN = 0;			// ×÷Îªslave¿¿½üºó²Ù×÷µÄÊ¹ÄÜ
tByte slave_nearby_operation_count = 0;	// ×÷Îªslave¿¿½üºó²Ù×÷µÄ´ÎÊý
tByte wire_broken_count = 0;		// ×÷Îª¶ÏÏßºóµÄÊ±¼ä¼ì²â
bit battery_stolen_EN = 0;			// ×÷Îªµç³Ø±»µÁµÄÊ¹ÄÜ¶Ë
tByte battery_stolen_count = 0;	// ×÷Îªµç³Ø±»µÁµÄ±¨¾¯´ÎÊý
bit horizontal_vibration = 0;		// ±íÊ¾´¹Ö±´«¸ÐÆ÷ÔÚÕñ¶¯£¬´ËÊ±¾ÍËã¹ØÔ¿³×£¬Ò²²»ÄÜÖ´ÐÐ¹ØÔ¿³×µÄ²Ù×÷¡
tWord horizontal_vibration_count = 0;	//´¹Ö±´«¸ÐÆ÷´¥·¢ºó£¬¶ÔÊ±¼ä½øÐÐ¼ÆÊý¡£

/*------------------------------------------------------------------
	timerT0()
	operation every ticket.
--------------------------------------------------------------------*/

void timer0() interrupt interrupt_timer_0_overflow
	{
	// manually reload timer0 configuration
	TH0 = timer0_8H;
	TL0 = timer0_8L;
	
	// timer0 is 1ms ticket, count the time flow of timer0, then operate every 2s.
	if(++timer0_count >= 2000)
		{

		// transmit the verification signal
		ComMode_1_Data(); 	
		
		// judge the communication control is on
		if(comm_whole_control == 1)
			{
			// if communication is on, increase the leave_count every 2s.
//			if((++leave_count >= 2)&&(slave_away == 0))
			if(++leave_count >= 2)
				{
				leave_count = 4;
				
				if((key_rotate == 1)&&(slave_away == 0))
					{
					if(nearby_away_interval > 6)
						{
						// turn off the magnet 
						magnet_ACW();
						nearby_away_interval = 0;
						
						// flag slave already away
						slave_away = 1;
						}
					}
				// enable position sensor(vertical and horizontal) and vibration sensor
				position_sensor_EN=1;	
				sensor_EN = 1;
            if(sensor_EN == 0)
					{
					// reset relatively sensor count
					sensor_trigger_count=0;
					sensor_1ststage_count=0;					
					}
				
				slave_nearby_EN = 0;
				slave_nearby_operation_count = 0;
				}
			}
		// reset timer0 ticket counter every 2s
		timer0_count=0;
		
		// detect the battery voltage
		ADC_check_result = GetADCResult(6);	
		
		// if fell and raised flag is 1, send alarm signal every 2s.
		if((fell_flag==1)&&(fell_alarm_count<5))
			{
			ComMode_5_Data();
			fell_alarm_count++;
			}
		if((raised_flag==1)&&(raised_alarm_count<5))		
			{
			ComMode_4_Data();
			raised_alarm_count++;
			}
			
		if(++nearby_away_interval > 6)
			{
			nearby_away_interval = 7;
			}
			
		// detect whether key is rotated on,  
		if((key_rotated_on_flag == 0)||(slave_away == 1))
			{
			if(key_rotate == 1)		
				{
				Delay(5);
				// anti-trigger, Delay(5) confirm the key rotation.
				if(key_rotate == 1)
					{
					if((slave_nearby_EN == 1)&&(slave_nearby_operation_count < 1))
						{
						slave_nearby_operation();
						// flag key rotation status
						key_rotated_on_flag = 1;
						}
					// reset the battery stolen flag
					battery_stolen_EN = 0;
					battery_stolen_count = 0;
					}
				leave_count = 0;
				} 
			}
		
		// detect whether key is rotated off
		if((key_rotate == 0)&&(key_rotated_on_flag == 1)&&(horizontal_vibration == 0))
			{
			Delay(5);
			if(key_rotate == 0)
				{
				// handle with battery status
				verifybattery(ADC_check_result);
				// reset key rotation flag
				key_rotated_on_flag=0;
				
				slave_away_operation();
				}
			
			leave_count = 0;
			}			
		
		// whether host has been touched 3 times, if yes, then alarm 2 speech alternantively.
		if((host_stolen_alarm1_EN == 1)&&(host_stolen_alarm1_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				ComMode_3_Data();
				stolen_alarm_speech1();
				}
			if(++host_stolen_alarm1_count >= 4)
				{
				host_stolen_alarm1_count = 0;
				host_stolen_alarm1_EN = 0;
				stolen_alarm_flag = 0;
				}
			}
		if((host_stolen_alarm2_EN == 1)&&(host_stolen_alarm2_count < 4))
			{
			stolen_alarm_flag = 1;
			if(key_rotate == 0)
				{
				ComMode_3_Data();
				stolen_alarm_speech2();
				}
			if(++host_stolen_alarm2_count >= 4)
				{
				host_stolen_alarm2_count = 0;
				host_stolen_alarm2_EN = 0;
				stolen_alarm_flag = 0;
				}
			}
		
		if((battery_stolen_EN == 1)&&(battery_stolen_count < 2))
			{
			ComMode_2_Data();
			battery_stolen_speech();
			battery_stolen_count++;
			}
		}		

	// judge host is fell or raised every 1ms?
	if(raised_fell_flag == 0)
		{
		// judge vibration sensor is enable?
		if(sensor_EN == 1)	
			{
			
			// sensor trigger status, 0, 1 or 2?
			switch(sensor_trigger_count)
				{
				// case 0, it means host is in waiting status. waiting for first touch.
				case 0:
					{
					
					// judge host been touched and also not in vibration alarm
//					if((sensor_detect == 0)&&(stolen_alarm_flag == 0)&&(transmiter_EN == 1))		
					if(((sensor_detect == 0)||(horizontal_sensor == 0))&&(stolen_alarm_flag == 0))		
						{
						// judge LV is more than 2ms, if yes, it means a effective touch
						if(++sensor_1ststage_count >= 1)			
							{
							sensor_1ststage_count=0;
							
							// sensor trigge status progress to case 1.
							sensor_trigger_count = 1;
							// alarm speech for first touoch
							host_touch_speech();
                     }
						}
					else
						{
						// if no LV, reset the count.
						sensor_1ststage_count = 0;
						}
					}
				break;
				
				// waiting for next touch, 
				case 1:
					{
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// LV for 2s, means a effective touch
						if(++sensor_2ndstage_count >= 1)
							{
							sensor_2ndstage_count = 0;
							sensor_trigger_count = 2;
							host_2ndtouch_speech_count = 0;
							// alarm speech for 2nd touch
							host_2ndtouch_speech();
							}
						}
					else
						{
						sensor_2ndstage_count = 0;
						}
					
					// if there is no touch in 4s, reset sensor trigger status, etc.
					if(++sensor_2ndstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_2ndstage_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_time = 0;
						}
					}
				break;
				
				// waiting for 3rd touch
				case 2:
					{
					if((sensor_detect == 0)||(horizontal_sensor == 0))
						{
						// 2s LV is a effective touch
						if(++sensor_3rdstage_count >= 1)
							{
							sensor_3rdstage_count = 0;
							// stolen alarm speech enable
							host_stolen_alarm1_EN = 1;
							host_stolen_alarm2_EN = 1;						
							}
						}
					else
						{
						sensor_3rdstage_count = 0;
						}
					
					// if there is no touch in 4s, reset all.
					if(++sensor_3rdstage_time >= 4000)
						{
						sensor_trigger_count = 0;
						sensor_1ststage_count = 0;
						sensor_2ndstage_count = 0;
						sensor_2ndstage_time = 0;
						sensor_3rdstage_time = 0;
						sensor_3rdstage_interval = 800;
						sensor_3rdstage_count = 0;
						sensor_3rdstage_effcount = 0;					
						}
					}
				break;
				}
			
			// judge the wire broken, if yes, it means someone has cut the wire of magnet lock
			if((wire_broken == 0) && (wire_broken_count < 51))  
				{
            if(++wire_broken_count > 50)
					{
					host_stolen_alarm1_EN = 1;
					host_stolen_alarm2_EN = 1;	
					wire_broken_count = 51;
               }
				}
			
			// judge whether battery has been stolen, 0.4V/4V
			if((ADC_check_result < 0x066) && (key_rotate == 0))
				{
				battery_stolen_EN = 1;
				}
			
			}
		}
	
	// judge whether position sensor is enable
	if(position_sensor_EN==1)		
		{
		// judge whether there is a LV
		if(raised_sensor_detect == 0)	
			{
			// LV > 0.5s means a effective input
			if(++raise_wire_time==10)
				{
				// flag raised, and reset fell
				raised_flag=1;
				fell_flag=0;
				// judge whether there once been a raised or fell.
				if(raised_fell_once_flag == 0)
					{
					// if no, flag raised and fell flag
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			raised_flag=0;
			raised_alarm_count=0;
			raise_wire_time=0;
			raised_fell_flag = 0;
			}
      // judge whether there is a LV
		if(fell_sensor_detect==0)
			{
			// LV > 0.5s means a effective input
			if(++fell_wire_time==10)	
				{
				// flag fell, and reset raised
				fell_flag=1;			
				raised_flag=0;
				// judge whether there once been a raised or fell
				if(raised_fell_once_flag == 0)
					{
					raised_fell_flag = 1;					
					}
				}		
			}
		else
			{
			fell_flag=0;
			fell_alarm_count=0;
			fell_wire_time=0;
			raised_fell_flag = 0;
			}
		}
	
	// judge whether raised or fell, if yes, reset all after 10s.
	if(raised_fell_flag == 1)
		{
		if(++raised_fell_number >= 4000)
			{
			raised_fell_flag = 0;
			raised_fell_number = 0;
			raised_fell_once_flag = 1;
			
			sensor_trigger_count = 0;
			sensor_1ststage_count = 0;
			sensor_2ndstage_count = 0;
			sensor_2ndstage_time = 0;
			sensor_3rdstage_time = 0;
			sensor_3rdstage_interval = 800;
			sensor_3rdstage_count = 0;
			sensor_3rdstage_effcount = 0;					
			}
		}
	
	// detect the horizontal sensor
	if(((horizontal_sensor == 0)||(sensor_detect == 0))&&(horizontal_vibration_count > 5000))
		{
		Delay(3);
		if((horizontal_sensor == 0)||(sensor_detect == 0))
			{
			horizontal_vibration = 1;
			horizontal_vibration_count = 0;
			}
		}
	if(++horizontal_vibration_count >= 5000)
		{
		horizontal_vibration_count = 5001;
		horizontal_vibration = 0;
		}
 	}


/*--------------------------------------------------------------------
	timerT1()
	¶¨Ê±Æ÷1Ã¿´ÎÒç³öºóÖ´ÐÐµÄ²Ù×÷
--------------------------------------------------------------------*/

void timerT1() interrupt interrupt_timer_1_overflow
	{
   // reload timer1
	TH1 = timer1_8H;				
	TL1 = timer1_8L;
	
	// receive a tyte
	receive_byte();
	
	// receive a word after every byte
	receive_word();
	
	// judge whether a byte has received successfully
	if(receive_data_finished_flag == 1)
		{
		receive_data_finished_flag = 0;
		// judge which mode is received.
		switch(received_data_buffer[2]) 		
			{
			case ComMode_1:  
				{
				if((nearby_away_interval > 6)||(key_rotate == 1))
					{
					// if receive a ComMode_1 successfully, operation for slave nearby
					slave_nearby_EN = 1;
					nearby_away_interval = 0;					
					}
				leave_count = 0;
				
				// turn off vibration and position sensor
				sensor_EN = 0;							
				position_sensor_EN=0; 			
				// reset all the flag in sensor
				fell_flag=0;  
				raised_flag=0;
				sensor_trigger_count = 0;
				sensor_1ststage_count = 0;
				sensor_2ndstage_count = 0;
				sensor_2ndstage_time = 0;
				sensor_3rdstage_time = 0;
				sensor_3rdstage_interval = 0;
				sensor_3rdstage_count = 0;
				sensor_3rdstage_effcount = 0;
				stolen_alarm_flag = 0;
				host_stolen_alarm1_EN = 0;
				host_stolen_alarm1_count = 0;
				host_stolen_alarm2_EN = 0;
				host_stolen_alarm2_count = 0;
				
				raised_fell_flag = 0;
				
				// reset the wire broken count
				wire_broken_count = 0;
				
				// reset the battery stolen flag
				battery_stolen_EN = 0;
				battery_stolen_count = 0;
				}
			break;
			}
		}
	}

/*--------------------------------------------------
	InitTimer()
	
	³õÊ¼»¯¶¨Ê±Æ÷T0ºÍT1
---------------------------------------------------*/

void InitTimer(const tByte Tick_ms_T0, Tick_us_T1)
	{
	tLong Inc_T0, Inc_T1;
	tWord timer0_16, timer1_16;
	
	//¼ÆËãTimer0µÄ¼Ä´æÆ÷Öµ
	Inc_T0 = (tLong)Tick_ms_T0 * (OSC_FREQ/1000) / (tLong)OSC_PER_INST;
	timer0_16 = (tWord) (65536UL - Inc_T0);
	timer0_8H = (tByte) (timer0_16 / 256);
	timer0_8L = (tByte) (timer0_16 % 256);
	
	//¼ÆËãTimer1µÄ¼Ä´æÆ÷Öµ
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

/*---------------------------------------------------------------------
	sEos_Go_To_Sleep()
	½«MCU½øÈëÐÝÃß×´Ì¬
----------------------------------------------------------------------*/
void sEOS_Go_To_Sleep(void)
   {
   PCON |= 0x01;    // Enter idle mode (generic 8051 version)
   }
	
/*---------------------------------------------------
	end of file
----------------------------------------------------*/