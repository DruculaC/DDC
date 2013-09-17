
//#define timer0H 0x24  
//#define timer0L 0x45  	//现在这个设置在13.5MHz的晶振下为0.05s

#define timer0H 0xfb  
#define timer0L 0x9b  	//现在这个设置在13.5MHz的晶振下为1ms
//设定需要定时的值为Tms，机器周期为(晶振频率/12)，则计算(65536-1000*T/(12/晶振频率))，将其值转换为16进制，即为TH和TL的设定值。

void InitT0();			//初始化定时器0




