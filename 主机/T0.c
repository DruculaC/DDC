
 #include"N79E81x.h"
#include"T0.h"


void InitT0()
{
	TMOD=0x21;//定时器0采用1方式
	TH0=timer0H;
	TL0=timer0L;
	TR0=1;
}


