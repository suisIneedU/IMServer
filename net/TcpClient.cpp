#include"TcpClient.h"
#include"../mediator/TcpClientMediator.h"

TcpClient::TcpClient(INetMediator* p):m_sock(INVALID_SOCKET), m_handle(nullptr), m_bRunning(true){
	m_pNetMediator = p;
}
TcpClient::~TcpClient() {

}
//��ʼ������(���ؿ⡢�����׽��֡����ӷ������������������ݵ��߳�)
bool TcpClient::initNet() {
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
	m_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sock) {
		cout << "socket error " << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "socket sucess " << endl;
	}
	//3.���ӷ�����
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	err = connect(m_sock,(sockaddr*)&addrServer, sizeof(addrServer));
	if (SOCKET_ERROR == err){
		cout<<"connect error: " <<WSAGetLastError() << endl;
		return false;
	}
	else{
		cout<<"connect success " << endl;
	}
	
	//4.�����������ݵ��߳�
	m_handle = (HANDLE)_beginthreadex(NULL,0,&recvThread,this,0,NULL);
	return true;
}

//�������ݵ��̺߳���������recvData��
unsigned __stdcall TcpClient::recvThread(void* lpVoid) {
	TcpClient* pThis = (TcpClient*)lpVoid;
	pThis->recvData();
	return 1;
}


//�ر�����(�����߳���Դ���ر��׽��֣�ж�ؿ�)
void TcpClient::unInitNet() {
	//1.�����̹߳���(��ѭ���Զ��˳�)����������һ��
	m_bRunning = false;

	//2.�رվ������������һ��
	if (m_handle) {
		if (WAIT_TIMEOUT  ==WaitForSingleObject(m_handle, 500)) {
			TerminateThread(m_handle, -1);
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//3.�ر��׽���
	if (m_sock && INVALID_SOCKET != m_sock) {
		closesocket(m_sock);
	}

	//4.ж�ؿ�
	WSACleanup();
}

//�������ݣ�ֻ�ܷ�������ˣ�
bool TcpClient::sendData(char* data, int len, unsigned long to) {
	//1.�жϲ����ĺϷ���
	if (!data || len<=0) {
		cout<<"TcpClient::sendData paramater error" << endl;
		return false;
	}
	//2.�ȷ�������
	int sendNum = send(m_sock, (char*)&len, sizeof(len), 0);
	if (SOCKET_ERROR == sendNum) {
		cout<<"TcpClient::sendData send error" <<WSAGetLastError<< endl;
		return false;
	}
	//3.�ٷ�������
	sendNum = send(m_sock, data, len, 0);
	if (SOCKET_ERROR == sendNum) {
		cout << "TcpClient::sendData send error" << WSAGetLastError << endl;
		return false;
	}
	return true;
}

//��������
void TcpClient::recvData() {
	//����recv�ķ���ֵ
	int recvNum = 0;
	//������Ĵ�С
	int packSize = 0;
	//��¼һ�������ۼƽ��յ��˶�������
	int offset = 0;
	while (m_bRunning) {
		//���հ���С
		recvNum =recv(m_sock, (char*)&packSize, sizeof(packSize), 0);
		if (recvNum >0) {
			//���հ����ȳɹ������հ�����
			//���հ��Ĵ�С������հ����ݵĿռ�
			char* packBuf = new char[packSize];
			//��ʼ���հ�����
			while (packSize/*��¼���л�ʣ���������*/>0) {
				recvNum/*��ʾһ�ν��յ���������*/ =recv(m_sock, packBuf + offset, packSize, 0);
				if (recvNum >0){
					//����һ�����ݳɹ����ۼ����������ӣ�ʣ��ռ����
						offset += recvNum;
					packSize -= recvNum;
				}
				else {
					cout<<"TcpClient::recvData recv2 error" << endl;
					break;
				}
			}
			//����ѭ����˵��һ���������ݽ������
			//�ѽ��յ������ݷ����н������
			m_pNetMediator->recvData(packBuf, offset,m_sock);
			
			//offset����
			offset = 0;
		}
		else {
			cout<<"TcpClient::recvData	reval error: " <<WSAGetLastError() << endl;
			break;
		}
	}
}