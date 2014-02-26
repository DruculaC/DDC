/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------

#define CmdHead 0xc8
#define MyAddress 0xe0

#define ComMode_1 0xc1	// the verification/confirmation signal. once slave receive this signal, slave transmit a confirmation signal.
								// once host receive this signal, it knows slave is here.
#define ComMode_2 0xc2 	// battery stolen signal out
#define ComMode_3 0xc3 	// host stolen alarm signal 
#define ComMode_4 0xc4 	// raised alarm signal
#define ComMode_5 0xc5	// fell alarm signal

//---------public function prototype

void initsignal(void);
void initsignal2(void);
void initsignal3(void);
void ComMode_1_Data(void);
void ComMode_2_Data(void);	
void ComMode_3_Data(void);	
void ComMode_4_Data(void);
void ComMode_5_Data(void);
void receive_byte(void);
void receive_word(void);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/