#pragma once
#include "INet.h"

class Udp :public INet {
public:
	Udp(INetMediator* p);
	~Udp();
	//��ʼ������
	bool initNet();

	//�ر�����
	void unInitNet();

	//��������
	bool sendData(char* data, int len, unsigned long to);

	//��������
	void recvData();

	// �������ݵ��̺߳���
	//beginthreadexʹ��recvThread������ʱ��û�д�����������recvThread��Ҫ����������ʹ�ã��Ͷ���ɾ�̬��
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};