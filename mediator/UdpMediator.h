#pragma once
#include"INetMediator.h"
class UdpMediator: public INetMediator {
public:
	UdpMediator();
	~UdpMediator();

	//������
	 bool openNet();

	//�ر�����
	 void closeNet();

	//��������
	 bool sendData(char* data, int len, unsigned long to);

	//��������
	//Tcp:socket��������˭���͹���������
	//Udp:ip��������˭���͹���������
	 void recvData(char* data, int len, unsigned long from);
};