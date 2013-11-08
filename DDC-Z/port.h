/*----------------------------------------------------
	port.h (v1.00)

	'port header' (see chap 5) for DDC-Z
----------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#include "main.h"

// P2.4，PIN15，连接到钥匙转动，钥匙转开时，此pin为1；钥匙关闭时，此pin为0
sbit key_rotate = P2^4;			

// P2.6，PIN27，发射机的功率控制。1表示小功率，0表示大功率。大功率发射距离远。
sbit transmiter_power = P2^6;

// P2.7，PIN28，发射机使能端口。1表示开，0表示关
sbit transmiter_EN = P2^7;

// P0.7，PIN19，传感器的检测结果，通过此PIN传给MCU，然后MCU判断
sbit sensor_detect = P0^7;

// P2.5，PIN16，传感器的使能控制，1表示打开，0表示关闭
sbit sensor_EN=P2^5;

// P2.2，P2.3，分别为PIN13，PIN14，电磁铁,  平时值为00，开锁用10，关锁有01，然后都恢复到00
sbit MagentControl_1 = P2^2;
sbit MagentControl_2 = P2^3;

// P0.4，PIN23，抬起传感器的检测
sbit raised_sensor_detect = P0^4;

// P0.3，PIN24，倒地传感器的检测
sbit fell_sensor_detect = P0^3;

//P1.4，PIN10，功放开关控制，1为打开功放，0为关闭功放
sbit voice_EN = P1^4;

// P2.0，P2.1分别为PIN1，PIN2，对应语音芯片的复位和数据管脚
sbit SC_RST=P2^0;
sbit SC_DATA=P2^1; 

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/