#include"TcpServerMediator.h"
#include"../net/TcpServer.h"
#include"../CKernel.h"

TcpServerMediator::TcpServerMediator() {
	m_pNet = new TcpServer(this);
}
TcpServerMediator::~TcpServerMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

	//������
bool TcpServerMediator::openNet() {

	return m_pNet->initNet();
}

	//�ر�����
void TcpServerMediator::closeNet() {
	m_pNet->unInitNet();
}

	//��������
bool TcpServerMediator::sendData(char* data, int len, unsigned long to) {

	return m_pNet->sendData(data, len, to);
}

	//��������
void TcpServerMediator::recvData(char* data, int len, unsigned long from) {
	//�ѽ��յ������ݴ���������
	CKernel::pKernel->dealData(data,len,from);
}
