#include"TcpServer.h"
#include"../mediator/TcpServerMediator.h"
TcpServer::TcpServer(INetMediator* p):m_sock(INVALID_SOCKET), m_bRunning(true) {
    m_pNetMediator = p;
}
TcpServer::~TcpServer() {

}

//��ʼ������(���ؿ⡢�����׽��֡���ip�˿ڡ������������������ӵ��߳�)
bool TcpServer::initNet() {
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

    //3����ip�Ͷ˿ں�
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    // ħ������:�����ֶ���ɺ�
    addr.sin_port = htons(_DEF_TCP_PORT);    // ת���������ֽ���(���)
    // ����������
    addr.sin_addr.S_un.S_addr = ADDR_ANY;
    err = bind(m_sock, (sockaddr*)&addr, sizeof(addr));
    if (SOCKET_ERROR == err) {
        cout << "bind error:" << WSAGetLastError() << endl;
        return false;
    }
    else {
        cout << "bind success" << endl;
    }
    //4.����
    err = listen(m_sock, _DEF_TCP_LISTEN_MAX);
    if (SOCKET_ERROR == err) {
        cout << "listen error: " << WSAGetLastError() << endl;
        return false;
    }else {
        cout << "listen success" << endl;
    }
    // 5.�������ӵ��̵߳ĺ���
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, &acceptThread, this, 0, NULL);
    if(handle) {
        m_listHandle.push_back(handle);
    }
    return true;
}

//�������ӵ��̺߳���(ѭ����������)
unsigned __stdcall TcpServer::acceptThread(void* lpVoid) {
    TcpServer* pThis = (TcpServer*)lpVoid;
    SOCKET sock = INVALID_SOCKET;
    sockaddr_in addrFrom ;
    int addrSize = sizeof(addrFrom);
    HANDLE handle = nullptr;
    unsigned int threadId = 0;
    while (pThis->m_bRunning) {
        //��������	
        sock = accept(pThis->m_sock, (sockaddr*)&addrFrom, &addrSize);
        if (INVALID_SOCKET == sock) {
            //��������ʧ��
            cout<<"accept error:" <<WSAGetLastError()<< endl;
            }else{
            //���ӳɹ����ȴ�ӡ�ͻ��˵� IP
            cout<<"accept success:" <<inet_ntoa(addrFrom.sin_addr)<< endl;

            //����ǰ���ӳɹ��Ŀͻ��˴���һ���������ݵ��߳�
            handle = (HANDLE)_beginthreadex(nullptr, 0, &recvThread, pThis, 0, &threadId);
            //�����߳̾��
            if (handle) {
                pThis->m_listHandle.push_back(handle);
            }
            //�����߳�id�Ϳͻ���socket
            pThis->m_mapThreadIdToSocket[threadId] = sock ;
        }
    }
    return 1;
}


//�������ݵ��̺߳���
unsigned __stdcall TcpServer::recvThread(void* lpVoid) {
    TcpServer* pThis = (TcpServer*)lpVoid;
    pThis->recvData();
    return 1;
}

//�ر�����(�����߳���Դ���ر��׽���n+1����ж�ؿ�)
void TcpServer::unInitNet(){
    //1.�����̹߳���(��ѭ���Զ��˳�)����������һ��
    m_bRunning = false;

    //2.�رվ������������һ��
    HANDLE handle = nullptr;
    for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
        //ȡ���ڵ��еľ��
        handle = *ite;
        //���վ����Ӧ����Դ
        if (handle) {
            if (WAIT_TIMEOUT == WaitForSingleObject(handle, 500)) {
                TerminateThread(handle, -1);
            }
            CloseHandle(handle);
            handle = nullptr;
        }
        //�Ƴ���ǰ������ָ�����Ч�ڵ�(����ֵ����һ����Ч�ڵ�)
        ite=m_listHandle.erase(ite);
    }
    if (handle) {
        if (WAIT_TIMEOUT == WaitForSingleObject(handle, 500)) {
            TerminateThread(handle, -1);
        }
        CloseHandle(handle);
        handle = nullptr;
    }

    //3.�ر��׽���
    if (m_sock && INVALID_SOCKET != m_sock) {
        closesocket(m_sock);
    }
    SOCKET sock = INVALID_SOCKET;
    for (auto ite = m_mapThreadIdToSocket.begin(); ite != m_mapThreadIdToSocket.end();) {
        //ȡ��socket
        sock = ite->second;
        //����socket
        if (sock&&INVALID_SOCKET != sock) {
            closesocket(sock);
        }
        //�Ƴ���ǰ������ָ�����Ч�ڵ�(����ֵ����һ����Ч�ڵ�)
        ite=m_mapThreadIdToSocket.erase(ite);
    }
    //4.ж�ؿ�
    WSACleanup();
}

//��������(�뷢��˭����˭)
bool TcpServer::sendData(char* data, int len, unsigned long to) {
    //1.У������ͷ���
    if (!data ||len<=0) {
        cout << "TcpServer::sendData paramater error" << endl;
        return false;
    }
    //2.�ȷ�������
    int sendNum = send(to,(char*)&len, sizeof(len), 0);
    if (sendNum == SOCKET_ERROR) {
        cout<<"TcpServer::sendData send1 error" <<WSAGetLastError() << endl;
        return false;
    }
    //3.�ٷ�������
    sendNum = send(to, data, len, 0);
    if (SOCKET_ERROR == sendNum) {
        cout<< "TcpServer::sendData send2 error" << WSAGetLastError()<<endl;
        return false;
    }
    return true;
}

//��������(���������������߳���������)
void TcpServer::recvData() {

    //����һ���(��acceptThreadһ��ʱ���socket���浽map�У���Ȼ�����map��ȡֵ��ʱ����ܻ�ʧ��)
    Sleep(5);
    //ȡ����ǰ�̶߳�Ӧ��socket
    unsigned int threadId =	GetCurrentThreadId();
    //��map��ȡֵ��ʱ������Ҫ�ж�map���Ƿ���key
    SOCKET sock = INVALID_SOCKET;
    if (m_mapThreadIdToSocket.count(threadId)>0) {
        sock = m_mapThreadIdToSocket[threadId];
    }
    //�жϻ�ȡ����socket�Ƿ���Ч
    if (!sock||INVALID_SOCKET==sock) {
        cout<<"TcpServer::recvData socket error" << endl;
        return;
    }


    //����recv�ķ���ֵ
    int recvNum = 0;
    //������Ĵ�С
    int packSize = 0;
    //��¼һ�������ۼƽ��յ��˶�������
    int offset = 0;
    while (m_bRunning) {
        //���հ���С
        recvNum = recv(sock, (char*)&packSize, sizeof(packSize), 0);
        if (recvNum > 0) {
            //���հ����ȳɹ������հ�����
            //���հ��Ĵ�С������հ����ݵĿռ�
            char* packBuf = new char[packSize];
            //��ʼ���հ�����
            while (packSize/*��¼���л�ʣ���������*/ > 0) {
                recvNum/*��ʾһ�ν��յ���������*/ = recv(sock, packBuf + offset, packSize, 0);
                if (recvNum > 0) {
                    //����һ�����ݳɹ����ۼ����������ӣ�ʣ��ռ����
                    offset += recvNum;
                    packSize -= recvNum;
                }
                else {
                    cout << "TcpServer::recvData recv2 error" << endl;
                    break;
                }
            }
            //����ѭ��
            //�ѽ��յ������ݷ����н������
            m_pNetMediator->recvData(packBuf, offset, sock);
    
            //offset����
            offset = 0;
        }
        else {
            cout << "TcpServer::recvData reval error: " << WSAGetLastError() << endl;
            break;
        }
    }
}
