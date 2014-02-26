/*---------------------------------------------------
	main.c (v1.00)
	
	DDC-Z program, for electrocar.
----------------------------------------------------*/

//电动车
#include "Main.h"
#include "port.h"

#include "AD.h"
#include "voice.h"
#include "pwm.h"
#include "Timer.h"
#include "Delay.h"
#include "communication.h"
#include "Battery.h"
#include "Other.h"
#include "operation.h"
                                        
/*------- Public variable declarations --------------------------*/
extern bit stolen_alarm_flag;		//主机被盗报警标志，1的时候表示触发		
extern bit host_stolen_speech_EN;		//主机被盗语音使能，1的时候在main中语音提示
extern bit battery_check;		         //置1时，执行一次电量转换，执行完后，将其置0
extern bit position_sensor_EN;  			//位置传感器，即倒地抬起传感器总开关，1的时候，检测这两个传感器
extern bit slave_away_speech_EN;      	//判断附机离开后，语音提示，在main里面操作
extern bit magnet_CW_EN;					//电磁铁顺时针转动使能，转动一次后复位为0
extern bit comm_whole_control;				//通信总开关，0关闭通信，1打开通信
extern tWord host_stolen_speech_count;    //判断主机被盗后，语音提示的次数
extern bit magnet_ACW_EN;						//电磁铁逆时针转动使能，转动一次后复位为0
extern bit slave_nearby_speech_EN;       //判断附近靠近后，语音提示，在main里面操作
extern bit host_stolen_alarm1_EN;      //判断为被盗报警后的第一段语音使能
extern bit host_stolen_alarm2_EN;      //判断为被盗报警后的第二段语音使能
extern tByte host_stolen_alarm1_count;		//判断为被盗报警后的第一段语音次数
extern tByte host_stolen_alarm2_count;		//判断为被盗报警后的第二段语音次数
extern bit fell_flag;						//判断主机倒下后，置1
extern tByte slave_away;					//模式选择位，1则用模式1,2则用模式2,3则为模式3
extern tByte leave_count;					//附机离开时的计数，只要附机3s应答正确，就清0	
extern bit raised_flag;					//判断主机被抬起后，置1
extern tByte sensor_2ndstage_LV_time; 	//传感器进入第二阶段检测时，计算低电平的时间
extern tByte sensor_2ndstage_count;		//传感器进入第二阶段检测时，计算低电平的时间
extern tByte host_2ndtouch_speech_count;		//主机被第二次触碰后，语音提示的次数
extern tWord sensor_3rdstage_time;			//传感器进入第三阶段的时间，
extern tByte sensor_3rdstage_effcount;		//传感器进入第三阶段后，有效触碰次数的计数
extern tByte sensor_3rdstage_count;			//传感器进入第三阶段后，低电平的计数
extern tWord sensor_3rdstage_interval;		//传感器在第三阶段中，每次有效低电平计数之间的时间间隔。在这期间的低电平不认为有效。
extern bit host_touch_speech_EN;				//第一次触碰后语音使能
extern bit host_2ndtouch_speech_EN;			//第二次触碰后语音使能
extern bit raised_fell_flag;					//倒地或者抬起触发后，此标志位置1
extern tByte sensor_trigger_count;			//传感器触发计数
extern tWord sensor_2ndstage_time;			//传感器进入第二阶段后流逝时间的计数
extern tByte sensor_1ststage_count;			//传感器第一阶段判断低电平的计数
extern tByte nearby_away_interval;		//附机离开和靠近时，语音提示和开关锁的时间间隔
extern bit magnet_ACW_flag;
extern bit slave_nearby_EN;			// 作为slave靠近后操作的使能
extern tByte key_rotated_on_flag;			//电动车开启关闭标志位，1表示电动车开启了，0表示电动车关闭了
extern tByte slave_nearby_operation_count;	// 作为slave靠近后操作的次数
extern tWord ADC_check_result;		//作为AD检测值

/*------- Private variable declarations --------------------------*/

void main()
	{
	InitTimer(1,100);
	
	sensor_EN = 0;
 	position_sensor_EN = 0;

	noVoice();
	myPwm();	//开发射机

	voice_EN=0;		//将功放关闭

	raised_sensor_detect=1;
	fell_sensor_detect=1;

	P10=1;
	
	magnet_ACW_flag=0;
   horizontal_sensor = 1;
	// open communication
	comm_whole_control = 1; 
	
	// turn off transmitter, turn on receiver
	transmiter_EN = 1;
	receiver_EN = 0;


	nearby_away_interval = 6;			//初始化使附机和主机开关机的计数为可以执行的状态
	
	// initialize the magnet, 
	MagentControl_1 = 1;
	MagentControl_2 = 1;
	
	transmiter_power = 1; 
   
	// lock the external motor, 防止锁还没完全打开的时候，车手加电导致轮子与锁的告诉碰撞。 
	motor_lock = 1;
	
	while(1)
		{
		sEOS_Go_To_Sleep();
		}
	}



/*---------------------------------------------------
	end of file
----------------------------------------------------*/