#include "Udp.h"
#include"../mediator/UdpMediator.h"

Udp::Udp(INetMediator* p):m_sock(INVALID_SOCKET),m_handle(nullptr),m_bRunning(true) {
	m_pNetMediator = p;
}

Udp::~Udp() {
	
}

	//��ʼ������(���ؿ⡢�����׽��֡���ip�Ͷ˿ںš������������ݵ��߳�)
bool Udp::initNet() {

	//1.���ؿ�
	WORD version = MAKEWORD(2, 2);
	WSADATA data = {};
	int err = WSAStartup(version, &data);
	if (err != 0) {
		cout << "WSAStartup failed " << endl;
		return false;
	}

	if (2 != HIBYTE(data.wVersion) || 2 != LOBYTE(data.wVersion)) {
		cout << "WSAStartup version error " << endl;
		return false;
	}
	else {
		cout << "WSAStartup success " << endl;
	}

	//2.�����׽���
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error " << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket sucess " << endl;
	}

	//3����ip�Ͷ˿ں�
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	// ħ������:�����ֶ���ɺ�
	addr.sin_port = htons(_DEF_UDP_PORT);    // ת���������ֽ���(���)
	// ����������
	addr.sin_addr.S_un.S_addr = ADDR_ANY;
	err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "bind error:" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "bind success" << endl;

		//4�������������ݵ��߳�
		// CreateThread��ExitThread��һ�ԣ�
		// ������߳��е���C++����ʱ��(����strcpy)������������ռ䲻�ͷ�,ExitThread���˳��̵߳�ʱ��Ҳ�����ͷſռ䣬�ͻ�����ڴ�й©

		// beginthreadex��_endthreadex��һ�ԣ�
		//_endthreadex���Ȼ��տռ䣬�ٵ���ExitThread���Ͳ���й¶�ڴ�
		m_handle = (HANDLE)_beginthreadex(NULL/*��ȫ����NULL����ʹ��Ĭ�ϵİ�ȫ����*/,
			0/*�̶߳�ջ��С��0����ʹ��Ĭ�϶�ջ��С1M*/,
			&recvThread/*�̺߳�������ʼ��ַ���̺߳����������߳���Ҫִ�еĺ���*/,
			this/*�̺߳����Ĳ����б�*/,
			0/*�̳߳�ʼ״̬��0�Ǵ��������У�CREATE_SUSPENDED�ǹ���״̬����Ҫ����ResumeThread ��ִ�д��߳�*/,
			NULL/*�߳�id�����������������Ҫ���Բ���*/);

		return true;
	}
}


	//�������ݵ��̺߳���
	unsigned __stdcall Udp::recvThread(void* lpVoid) {
		Udp* pThis = (Udp*)lpVoid;
		pThis->recvData();
		return 1;
	}

			
	//�ر�����(�����߳���Դ���ر��׽��֡�ж�ؿ�)
void Udp::unInitNet() {
	//�����̵߳�ʱ�򣬲���ϵͳ���ÿ���̷߳���������Դ���߳�ID,�߳̾�����ں˶��󡣳�ʼ�����ü�������2
	//����ϵͳ������Դ�����ü������ķ�ʽ�����ü�����Ϊ0��ʱ���Զ�����
	//1.�����̹߳���(��ѭ���Զ��˳�)����������һ��
	m_bRunning = false;
	
	//2.�رվ������������һ��
	if (m_handle) {
		if (WAIT_TIMEOUT /*�ȴ���ʱ��������ô��ʱ���̻߳�û���˳��Ļ�*/ ==
			WaitForSingleObject(m_handle/*�ȴ����ĸ��߳�*/, 500/*�ȴ��˶೤ʱ�䣬��λ�Ǻ���*/)) {
			//ǿ�ƽ����̣߳����ǲ�Ҫһ��ʼ��ǿ��,�س��������һЩ�������Դ������δ�ͷţ������˾ʹ򲻿��ˡ�
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}
	//3.�ر��׽���
	if (m_sock && INVALID_SOCKET!=m_sock) {
		closesocket(m_sock);
	}
	//4.ж�ؿ�
	WSACleanup();
}

	//��������
bool Udp::sendData(char* data, int len, unsigned long to) {
	// �ж����ݵĺϷ���
	if (!data||len <= 0)
	{
		cout << "Udp::snedData paramater error"<<endl;
		return false;
	}

	//��������
	sockaddr_in addrTo;
	addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(_DEF_UDP_PORT);
	addrTo.sin_addr.S_un.S_addr = to;
	int sendNum = sendto(m_sock,data,len,0,(sockaddr*)&addrTo,sizeof(addrTo));
	if (SOCKET_ERROR == sendNum) {
		cout << "Udp::sendData sendto error" <<WSAGetLastError() << endl;
		return false;
	}
	return true;
}

	//��������(��������,�����߳�������	)
void Udp::recvData() {
	char buf[8192]="";
	int recvNum = 0;
	sockaddr_in addrFrom;
	int addrSize = sizeof(addrFrom);
	while (m_bRunning) {
		recvNum = recvfrom(m_sock, buf, sizeof(buf), 0, (sockaddr*)&addrFrom, &addrSize);
		if (recvNum>0) {
			//�������ݳɹ�,���뵽��һ���µĿռ�
			char* packBuf = new char[recvNum];
			//�ѽ��յ������ݿ������¿ռ���
			//strcpyֻ�ܿ����ַ������ַ�����\0Ϊ������־����������������0��ֱ��ֹͣ
			// ��strcpy�������ݺܿ��ܵ�������0����ǰֹͣ���������ݵĲ�����
			// memcpyֻ���������ݵĴ�С���������е�����
			memcpy(packBuf,buf,recvNum);

			//���¿ռ�����ݴ����н�����,�����н�����ĺ���
			m_pNetMediator->recvData(packBuf, recvNum, addrFrom.sin_addr.S_un.S_addr);
		}
		else {
			cout << "Udp::recvData recvfrom error" <<WSAGetLastError() << endl;
			break;
		}
	}
}	
