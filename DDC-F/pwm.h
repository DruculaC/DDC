/*---------------------------------------------------
	PWM.h (v1.00)
	
	header file for PWM.c
----------------------------------------------------*/

#ifndef _PWM_H
#define _PWM_H

//-----public constants------------------------------

#define HIBYTE(v1) ((tByte)((v1)>>8))
#define LOBYTE(v1) ((tByte)((v1)&0xFF))

#define PWMP       0x1d		//频率 450kHz
#define PWM0_Duty  0x0d		//占空比
#define PWM1_Duty  0x100
#define PWM2_Duty  0x080
#define PWM3_Duty  0x040

typedef enum 
	{
    E_CHANNEL0,
    E_CHANNEL1,      
    E_CHANNEL2,      
    E_CHANNEL3,
	} E_PWMCNL_SEL;

//-----public function prototype---------------------

void Init_PWM(void)	;	//初始化PWM
void PWM_Channel(E_PWMCNL_SEL Channel);	//选择PWM输出通道
void myPwm();	//开启PWM

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/