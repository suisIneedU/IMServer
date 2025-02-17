#pragma once
#include "INet.h"
#include<map>
#include<list>
class TcpServer :public INet {
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//��ʼ������
	bool initNet();

	//�ر�����
	void unInitNet();

	//��������
	bool sendData(char* data, int len, unsigned long to);

	//��������
	void recvData();

	// �������ݵ��̺߳���
	static unsigned __stdcall acceptThread(void* lpVoid);
	static unsigned __stdcall recvThread(void* lpVoid);
private:
	SOCKET m_sock;
	bool m_bRunning;
	map<unsigned int, SOCKET> m_mapThreadIdToSocket;
	list<HANDLE> m_listHandle;
};