/*------------------------------------------------------------------
	port.h (v1.00)

	'port header' (see chap 5) for DDC-F
------------------------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#include "main.h"

//----- main.c ----------------------------------------------------

// P1.3，PIN11，接收机使能，1打开接收机，0关闭接收机，要加上拉电阻			
sbit Receiver_EN = P1^3;		

//----- main.c ------ communication.c ----------------------------

// P1.0，PIN18，发射机使能，1打开发射机，0关闭发射机 
sbit Transmitter_EN = P1^0;			

// ----- Other.C -------------------------------------------------

// P2.4，PIN15，振动器控制，0振动，1不振动
sbit Moto_EN = P2^4;	

// -------- voice.c ---------------------------------------------

// P1.4，PIN10，功放开关控制，1为打开功放，0为关闭功放
sbit Voice_EN = P1^4;

// P2.0，P2.1分别为PIN1，PIN2，对应语音芯片的复位和数据管脚
sbit SC_RST = P2^0;	
sbit SC_DATA = P2^1;

// ----- Timer.c -------------------------------------------------
// P1.1，PIN17，接收信号的输入线
sbit receive_wire = P1^1;

// P2.7，PIN28，   发射信号的输出线
sbit transmit_wire = P2^7;

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/