


/************** 端口定义 *****************/
sbit SC_RST=P2^0; //P2.0 是脉冲复位脚
sbit SC_DATA=P2^1; //P2.1 是脉冲数据脚




//延时 X 毫秒
void delay_ms(unsigned int count);

//延时 X 微秒
void delay_us(unsigned int count);
//控制地址段放音
void SC_Speech(unsigned char cnt);
//防止上电乱发声
void noVoice();

