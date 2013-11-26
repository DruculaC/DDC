/*-----------------------------------------------------
	communication.h (v1.00)
	
	header file for communication.c
-----------------------------------------------------*/

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H

//-----public constants--------------------------------

//ͨ�ű�������������ɣ�Head, Address��Command���
#define CmdHead 0xc8
#define MyAddress 0xe0

#define ComMode_1 0xc1	//ͨ��ģʽ1�������յ���ָ��󣬱��������ڸ�����û���ر����� 
#define ComMode_3 0xc3 	//ͨ��ģʽ3�������յ���ָ���������ʾ��������͵����
#define ComMode_4 0xc4 	//̧��ָ������յ���ָ���������ʾ���綯������̧��
#define ComMode_5 0xc5	//����ָ������յ���ָ���������ʾ���綯�����ء�
#define ComMode_6 0xc6	//�������ָ�������յ���ָ���������ʾ���綯���ĵ���Ѿ������硱

//---------public function prototype

void initsignal(void);
void initsignal2(void);
void initsignal3(void);
void ComMode_1_Data(void);
void ComMode_3_Data(void);	
void ComMode_4_Data(void);
void ComMode_5_Data(void);

#endif
											
/*---------------------------------------------------
	end of file
----------------------------------------------------*/