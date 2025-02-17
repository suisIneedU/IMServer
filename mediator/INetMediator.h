#pragma once
//��������INet����࣬�ñ�����ֱ��ʹ��,��Ϊ�������������,���Իᱻ���뵽
class INet;
class INetMediator {
public:
	INetMediator():m_pNet(nullptr) {}
	~INetMediator() {}

	//������
	virtual bool openNet() = 0;

	//�ر�����
	virtual void closeNet() = 0;
	 
	//��������
	virtual bool sendData(char* data, int len, unsigned long to) = 0;

	//��������
	//Tcp:socket��������˭���͹���������
	//Udp:ip��������˭���͹���������
	virtual void recvData(char* data ,int len, unsigned long from) = 0;
protected:
	INet* m_pNet;
};