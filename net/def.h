#pragma once
#include<string.h>

// ����UDPЭ��Ķ˿ں�
#define  _DEF_UDP_PORT (1234)

// ����TCPЭ��Ķ˿ں�
#define _DEF_TCP_PORT (4321)

// ����TCPЭ��ļ������������
#define _DEF_TCP_LISTEN_MAX			 (100)

//�ǳƺ��������󳤶�
#define _DEF_NAME_MAX_LENGTH		 (20)

//�绰�������󳤶�
#define _DEF_TEL_MAX_LENGTH			 (12)

//ǩ���������
#define _DEF_FEELING_MAX_LENGTH		 (100)

//�������������
#define _DEF_CHAT_CONTENT_MAX_LENGTH (4096)
//����ָ�������С��Э��ͷ������
#define _DEF_PROTOCOL_COUNT			 (10)
//����Э��ͷ
#define _PROTOCOL_BASE				 (1000)
//ע������
#define _DEF_REGISTER_RQ			 (_PROTOCOL_BASE + 1)
//ע��ظ�
#define _DEF_REGISTER_RS			 (_PROTOCOL_BASE + 2)
//��½����
#define _DEF_LOGIN_RQ				 (_PROTOCOL_BASE + 3)
//��½�ظ�
#define _DEF_LOGIN_RS				 (_PROTOCOL_BASE + 4)
//��Ӻ�������
#define _DEF_ADD_FRIEND_RQ			 (_PROTOCOL_BASE + 5)
//��Ӻ��ѻظ�
#define _DEF_ADD_FRIEND_RS			 (_PROTOCOL_BASE + 6)
//��ʾ�����б�
#define _DEF_FRIEND_INFO			 (_PROTOCOL_BASE + 7)
//��������
#define _DEF_CHAT_RQ				 (_PROTOCOL_BASE + 8)
//����ظ�
#define _DEF_CHAT_RS				 (_PROTOCOL_BASE + 9)
//��������
#define _DEF_OFFLINE_RQ				 (_PROTOCOL_BASE + 10)


//����������
//ע����
#define _def_register_success		(0)
#define _def_register_tel_exist		(1)
#define _def_register_name_exist	(2)

//��½���
#define _def_login_success			(0)
#define _def_login_tel_not_exist	(1)
#define _def_login_password_error	(2)

//��Ӻ��ѽ��
#define _def_add_friend_success		(0)
#define _def_add_friend_not_exist	(1)
#define _def_add_friend_refuse		(2)
#define _def_add_friend_offline		(3)

//�û�״̬
#define _def_status_online			(0)
#define _def_status_offline			(1)

//������
#define _def_chat_success			(0)
#define _def_chat_fail				(1)

//��������ṹ��
// ע������:�ֻ��� �ǳ� ����
typedef struct _STRU_REGISTER_RQ {
	_STRU_REGISTER_RQ():type(_DEF_REGISTER_RQ){
		memset(tel,0, _DEF_TEL_MAX_LENGTH);
		memset(name, 0, _DEF_NAME_MAX_LENGTH);
		memset(password, 0, _DEF_NAME_MAX_LENGTH);
	}
	int type;
	char tel[_DEF_TEL_MAX_LENGTH];
	char name[_DEF_NAME_MAX_LENGTH];
	char password[_DEF_NAME_MAX_LENGTH];
}_STRU_REGISTER_RQ;

//ע��ظ�:ע���� (ʧ��:�ֻ����ѱ�ע�ᣬ�ǳ��ѱ�ע��)���߳ɹ�
typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS():result(_def_register_name_exist),type(_DEF_REGISTER_RS) {}
	int type;
	int result;
}_STRU_REGISTER_RS;

// ��¼����:�绰���� ������
typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ():type(_DEF_LOGIN_RQ) {
		memset(tel, 0, _DEF_TEL_MAX_LENGTH);
		memset(password, 0, _DEF_NAME_MAX_LENGTH);
	}
	int type;
	char tel[_DEF_TEL_MAX_LENGTH];
	char password[_DEF_NAME_MAX_LENGTH];
}_STRU_LOGIN_RQ;

//��¼�ظ�:��½�Ľ��(�ɹ����ֻ���δע��,�������)
typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS() :result(_def_login_password_error),type(_DEF_LOGIN_RS) ,userId(0){}
	int type;
	int result;
	int userId;
}_STRU_LOGIN_RS;

// ��Ӻ�������:ֻ֧���ǳ���Ӻ���   ���ѵ��ǳơ��Լ����ǳơ��Լ���ID
typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ() :type(_DEF_ADD_FRIEND_RQ), userId(0) {
		memset(friendName, 0, _DEF_NAME_MAX_LENGTH);
		memset(userName, 0, _DEF_NAME_MAX_LENGTH);
	}
	int type;
	char friendName[_DEF_NAME_MAX_LENGTH];
	char userName[_DEF_NAME_MAX_LENGTH];
	int userId;
}_STRU_ADD_RQ;

//��Ӻ��ѻظ�����ӽ��(�ɹ����ǳƲ����ڡ��ܾ��ˡ�������),���ѵ��ǳ�,���ѵ�id���Լ���id
typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() :result(_def_add_friend_refuse),
		friendId(0),userId(0),type(_DEF_ADD_FRIEND_RS){
		memset(friendName,0, _DEF_NAME_MAX_LENGTH);
	}
	int type;
	int result;
	int userId;
	int friendId;
	char friendName[_DEF_NAME_MAX_LENGTH];
}_STRU_ADD_FRIEND_RS;
 
// ��ʾ�����б�:���ѵ�id���ǳƣ�ǩ����ͷ��id��״̬
typedef struct _STRU_FRIEND_INFO {
	_STRU_FRIEND_INFO():id(0), iconId(0), status(_def_status_offline),type(_DEF_FRIEND_INFO) {
		memset(feeling, 0, _DEF_FEELING_MAX_LENGTH);
        memset(name, 0, _DEF_NAME_MAX_LENGTH);	
	}
	int type;
	int id;
	int iconId;
	int status;
	char name[_DEF_NAME_MAX_LENGTH];
	char feeling[_DEF_FEELING_MAX_LENGTH];
}_STRU_FRIEND_INFO;
 
// ��������:��������,�Լ���id�����ѵ�id
typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ():type(_DEF_CHAT_RQ),userId(0), friendId(0) {
		memset(content, 0, _DEF_CHAT_CONTENT_MAX_LENGTH);
	}
	int type;
	char content[_DEF_CHAT_CONTENT_MAX_LENGTH];
    int userId;
    int friendId;
}_STRU_CHAT_RQ;

//����ظ�:���ͽ��(���ͳɹ�,����ʧ��)
typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS() :type(_DEF_CHAT_RS), result(_def_chat_fail), friendId(0) {}
	int type;
	int result;
	int friendId;
}_STRU_CHAT_RS;
 
// ��������:�Լ���id
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ),userId(0){}
	int type;
	int userId;
}_STRU_OFFLINE_RQ;

