#pragma once
#include "INet.h"

class TcpClient :public INet {
public:
	TcpClient(INetMediator* p);
	~TcpClient();
	//��ʼ������
	bool initNet();

	//�ر�����
	void unInitNet();

	//��������
	bool sendData(char* data, int len, unsigned long to);

	//��������
	void recvData();

	// �������ݵ��̺߳���
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	HANDLE m_handle;
	bool m_bRunning;
};