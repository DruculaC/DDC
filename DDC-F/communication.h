/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------

//通信编码由三部分组成，Head, Address和Command组成
#define CmdHead 0xc8
#define MyAddress 0xe0

#define ComMode_1 0xc1	//通信模式1，附机收到此指令后，表明主机在附近，没有特别意义 
#define ComMode_2 0xc2 	// battery stolen signal out
#define ComMode_3 0xc3 	//通信模式3，附机收到此指令后，语音提示“有人在偷车”
#define ComMode_4 0xc4 	//抬起指令，附机收到此指令后，语音提示“电动车被人抬起”
#define ComMode_5 0xc5	//倒地指令，附机收到此指令后，语音提示“电动车倒地”
#define ComMode_6 0xc6	//充电已满指，附机收到此指令后，语音提示“电动车的电池已经充满电”

//---------public function prototype

void ComMode_1_Data(void);		
void initsignal(void);				

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/