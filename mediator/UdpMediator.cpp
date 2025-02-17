#include"UdpMediator.h"
#include"../net/Udp.h"

UdpMediator::UdpMediator() {
	m_pNet = new Udp(this);
}

UdpMediator::~UdpMediator() {
	if (m_pNet) {
		m_pNet->unInitNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

	//������
bool UdpMediator::openNet() {

	return m_pNet->initNet();
}

	//�ر�����
void UdpMediator::closeNet() {
	m_pNet->unInitNet();
}

	//��������
bool UdpMediator::sendData(char* data, int len, unsigned long to) {

	return m_pNet->sendData(data, len, to);
}

	//��������
void UdpMediator::recvData(char* data, int len, unsigned long from) {
	//TODO:�ѽ��յ������ݴ���������
	//���Դ��� :��ӡ���յ�������
	cout<<"UdpMediator::recvData:"<<data <<" ,len:"<<len<<" ,ip:"<<from<< endl;
}
