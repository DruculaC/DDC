/*-----------------------------------------------------
	voice.h (v1.00)
	
	header file for voice.c
-----------------------------------------------------*/

#ifndef _VOICE_H
#define _VOICE_H

//-----public constants-------------------------------

//-----public function prototype----------------------

void SC_Speech(unsigned char cnt);
void noVoice();
void Alarm_stolen_speech(void);
void Alarm_raised_speech(void);
void Alarm_fell_speech(void);
void Battery_low_alarm_speech(void);
void Battery_high_alarm_speech(void);
void Host_battery_high_alarm_speech(void);
void battery_stolen_speech(void);


#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/






