#pragma once
#include"INetMediator.h"
class TcpServerMediator : public INetMediator {
public:
	TcpServerMediator();
	~TcpServerMediator();

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