/*---------------------------------------------------
	Other.c (v1.00)
	
	其他程序
---------------------------------------------------*/

#include "main.h"
#include "port.h"

#include "Other.h"
#include "Delay.h"

/*-------------------------------------------------------
	magnet_CW()
	电磁铁正转，顺时针，将锁打开
--------------------------------------------------------*/
void magnet_CW(void)
	{
	MagentControl_1 = 0;
	MagentControl_2 = 1;
	magnet_delay();
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	motor_lock = 0;
	}

/*-------------------------------------------------------
	magnet_ACW()
	电磁铁反转，逆时针，将锁关闭
--------------------------------------------------------*/
void magnet_ACW(void)
	{
	motor_lock = 1;
	MagentControl_1 = 1;
	MagentControl_2 = 0;
	magnet_delay();
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	}
/*---------------------------------------------------
	end of file
----------------------------------------------------*/