
#define timer1H 0xff  
#define timer1L 0x8e  //现在这个设置在13.5MHz的晶振下为100us

//设定需要定时的值为Tms，机器周期为(晶振频率/12)，则计算(65536-1000*T/(12/晶振频率))，将其值转换为16进制，即为TH和TL的设定值。

void InitT1();//初始化定时器01