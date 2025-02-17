#pragma once
#include<map>
#include<iostream>
#include"MySQL/CMySql.h"
#include"mediator/INetMediator.h"
#include"net/def.h"
using namespace std;

class CKernel;
//���庯��ָ������
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel
{
public:
	CKernel();
	~CKernel();

	//�򿪷����
	bool startServer();
	//�رշ����
	void closeServer();

	//��ʼ������ָ������
	void setProtocolFun();
	
	//��ȡ��ǰ��¼�û�����Ϣ�Լ����ѵ���Ϣ
	void getUserInfoAndFriendInfo(int userId);

	//����id��ѯ��Ϣ
	//         ���������Ҫ��ѯ���û�    ������������ز�ѯ�����û���Ϣ
	void getInfoById(int id,_STRU_FRIEND_INFO* info);	

	//�����ַ����н��յ�������
	void dealData(char* data, int len, unsigned long from);

	//�����¼����
	void dealLoginRq(char* data, int len, unsigned long from);

	//������������
	void dealOfflineRq(char* data, int len, unsigned long from);

	//����ע������
	void dealRegisterRq(char* data, int len, unsigned long from);

	//������������
	void dealChatRq(char* data, int len, unsigned long from);

	//������Ӻ�������
	void dealAddFriendRq(char* data, int len, unsigned long from);

	//������Ӻ��ѻظ�
	void dealAddFriendRs(char* data, int len, unsigned long from);

	static CKernel* pKernel;
    public:
private:
	INetMediator* m_pNetMediator;
    CMySql m_sql;
	//����ָ������
	PFUN m_vectorProtocolFun[_DEF_PROTOCOL_COUNT];
	//�����¼�û���id��socket(��½�ɹ���ʱ�򱣴棬���ߵ�ʱ��ɾ����
	//�ж��û��Ƿ����߾Ϳ����ж�map���Ƿ�������û���id)
	map<int,SOCKET> m_mapIdToSocket;
};

