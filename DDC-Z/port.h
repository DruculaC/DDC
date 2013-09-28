/*----------------------------------------------------
	port.h (v1.00)

	'port header' (see chap 5) for DDC-Z
----------------------------------------------------*/

#ifndef _PORT_H
#define _PORT_H

#include "main.h"

//设置报电量的按钮，当电动车钥匙转动后，报一次电量
//无论是从0到1，或者是1到0，但语音提示不一样。
sbit det_battery=P2^4;			

//sbit PWMout=P0^1;//发射机的方波输出口，使用外设PWM
sbit ModeControl_1=P2^6;//发射机模式控制,0亮为30M模式，1灭为300M模式
sbit tran_en=P2^7;//发射机开关，1亮为开了，0灭为关了

sbit ReceWave=P0^7;//三轴传感器输入端
sbit SensorControl=P2^5;//三轴传感器控制端

//设置充电指示
//sbit det_charge=P0^2;

//电磁铁,  平时值为00，开锁用10，关锁有01，然后都恢复到00
sbit MagentControl_1=P2^2;
sbit MagentControl_2=P2^3;

sbit upSignal=P0^4;//抬起信号
sbit downSignal=P0^3;//倒地信号

//电池控制 	AD的1号通道为电池的电量检测端
sbit BatteryControl=P1^2;

//功放开关控制，1为打开功放，0为关闭功放
sbit PAshutdown=P1^4;

sbit SC_RST=P2^0; //P2.0 是脉冲复位脚
sbit SC_DATA=P2^1; //P2.1 是脉冲数据脚

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/