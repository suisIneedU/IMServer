#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <Winsock2.h>
#include<process.h>
#include"def.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;
//��������INetMediator����࣬�ñ�����ֱ��ʹ��,��Ϊ�������������,���Իᱻ���뵽
class INetMediator;
class INet {
public:
	INet():m_pNetMediator(nullptr){}
	virtual ~INet() {}


	//��ʼ������
	virtual bool initNet() = 0;

	//�ر�����
	virtual void unInitNet() = 0;

	//��������
	virtual bool sendData(char* data, int len,unsigned long to) = 0;

	//��������
	virtual void recvData() = 0;

protected:
	INetMediator* m_pNetMediator;
};