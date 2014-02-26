/*-----------------------------------------------------
	voice.h (v1.00)
	
	header file for voice.c
-----------------------------------------------------*/

#ifndef _VOICE_H
#define _VOICE_H

//-----public constants-------------------------------

//-----public function prototype----------------------

void SC_Speech(unsigned char cnt);
void SC_Speech2(unsigned char cnt);
void noVoice();
void key_rotate_on_speech(void);
void motorBAT_low_speech(void);
//void key_rotate_off_speech(void);
void stolen_alarm_speech1(void);
void stolen_alarm_speech2(void);
void slave_nearby_speech(void);
void slave_away_speech(void);
void host_touch_speech(void);
void host_2ndtouch_speech(void);
void battery_stolen_speech(void);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/