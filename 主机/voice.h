



/************** �˿ڶ��� *****************/
sbit SC_RST=P2^0; //P2.0 �����帴λ��
sbit SC_DATA=P2^1; //P2.1 ���������ݽ�


//��ʱ X ����
void delay_ms(unsigned char count);




//��ʱ X ΢��
void delay_us(unsigned int count);




//���Ƶ�ַ�η���
void SC_Speech(unsigned char cnt);

void noVoice();//��ֹ�ϵ�ʱ���ҽе�����