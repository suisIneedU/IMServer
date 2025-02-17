#include "CKernel.h"
#include"mediator/TcpServerMediator.h"

//��̬��Ա���������ʼ��
CKernel* CKernel::pKernel =nullptr;

CKernel::CKernel():m_pNetMediator(nullptr){
	pKernel = this;
	setProtocolFun();
}

//��ʼ������ָ������
void CKernel::setProtocolFun(){
	cout<<__func__<<endl;

	//�����ʼ��
	memset(m_vectorProtocolFun,0,sizeof(m_vectorProtocolFun));

	//�Ѻ�����ַ���浽������
	m_vectorProtocolFun[_DEF_REGISTER_RQ	-_PROTOCOL_BASE-1] = &CKernel::dealRegisterRq;
	m_vectorProtocolFun[_DEF_LOGIN_RQ		-_PROTOCOL_BASE-1] = &CKernel::dealLoginRq;
	m_vectorProtocolFun[_DEF_OFFLINE_RQ		-_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_vectorProtocolFun[_DEF_CHAT_RQ		-_PROTOCOL_BASE - 1] = &CKernel::dealChatRq; 
	m_vectorProtocolFun[_DEF_ADD_FRIEND_RQ  -_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_vectorProtocolFun[_DEF_ADD_FRIEND_RS  -_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
}

CKernel::~CKernel() {
	if (m_pNetMediator) {
		m_pNetMediator->closeNet();
        delete m_pNetMediator;
		m_pNetMediator = nullptr;
	}
}

//�򿪷����
bool CKernel::startServer(){

	//������
	m_pNetMediator = new TcpServerMediator;
	if (!m_pNetMediator->openNet()) {
		cout << "�򿪷��������ʧ��" << endl;
		return false;
	}
	//TODO :�������ݿ�
	char ip[] = "127.0.0.1";
	char name[] = "root";
	char pass[] = "123456";
	char dbName[] = "im";
	if (!m_sql.ConnectMySql(ip, name, pass, dbName)){
		cout<<"�������ݿ�ʧ��" << endl;
		return false;
	}
	else {
		cout << "�������ݿ�ɹ�" << endl;
	}
	return true;
}
//�رշ����
void CKernel::closeServer(){
	if (m_pNetMediator) {
		m_pNetMediator->closeNet();
		delete m_pNetMediator;
		m_pNetMediator = nullptr;
	}
}

//�����ַ����н��յ�������
void CKernel::dealData(char* data, int len, unsigned long from) {
	cout<<__func__<<endl;
	//ȡ��type��ֵ
	int type = *(int*)data;
	//����type��ֵ���ж����ĸ��ṹ��,���ò�ͬ�Ĵ�����
	/*switch (type) {
		case _DEF_REGISTER_RQ:
	 		dealRegisterRq(data,len,from);
			break;
		case _DEF_LOGIN_RQ:
			dealLoginRq(data, len, from);
			break;
		default:
			break;
	}*/
	//����typeֵ���������±�
	int index = type - _PROTOCOL_BASE - 1;

	//�ж������±��Ƿ�Խ��
	if (index>= 0 && index < _DEF_PROTOCOL_COUNT) {
		//���������±�ȡ��������ַ
		PFUN pf = m_vectorProtocolFun[index];
		//�жϺ�����ַ�Ƿ�Ϊ�գ���Ϊ�վ͵��ö�Ӧ�Ĵ�����
		if (pf) {
			(this->*pf)(data, len, from);
		}
		else {
			cout<< "������ַΪ��,type:" <<type<< endl;
		}
	}
	else {
		//typeֵ�ر��:1��offsetû���� 2���ṹ�嶨���ʱ��û��ֵ 3�������ʱ��û�����ͷ
		cout << "type:" << type << endl;
	}
}

//����ע������
void CKernel::dealRegisterRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	
	//2��У�������Ƿ�Ϸ�

	//(1)�ж��ַ����Ƿ�Ϊ�ջ�����ȫ�ո�
	//if (name.isEmpty() || tel.isEmpty() || password.isEmpty() ||
	//	nameTmp.remove(" ").isEmpty() || telTmp.remove(" ").isEmpty() || passwordTmp.remove(" ").isEmpty()) {                                     //remove()��ı����ݵ�ԭʼ����
	//	QMessageBox::about(this, "��ʾ", "��������յ����ݻ���ȫ�ո�");
	//	return;
	//}
	//(2)�ж��ַ��������Ƿ�Ϸ�
	/*if (name.length() > 20 || password.length() > 20 || 11 != tel.length()) {
		return;
	}*/

	//3�����ݵ绰��������ݿ��в�ѯ
	list<string> lstStr;
	char szSql[1024] ="";
	sprintf_s(szSql, "select tel from t_user where tel ='%s';",rq->tel);
	if (!m_sql.SelectMySql(szSql,1,lstStr)) {
		//��ѯ���ݿ�ʧ�ܵ��ĸ�ԭ��:1�����ݿ�û�н���  2��δ���� 
		//3�����ӵĿ���û��Ҫ��ѯ��  4��sql������
		cout<<"��ѯ��ʧ��" << szSql<< endl;
		return;
	}

	_STRU_REGISTER_RS rs;
	// 4���жϲ�ѯ����Ƿ�Ϊ��
	if (0 !=lstStr.size()) {
		//5�������ѯ���Ϊ��Ϊ�գ�˵���ѱ�ע�� _def_register_tel_exit
		rs.result = _def_register_tel_exist;
	}
	else {
		//6�������ѯ���Ϊ�գ�˵��δ��ע��
		//�����ǳƴ����ݿ��в�ѯ
		sprintf_s(szSql, "select name from t_user where name ='%s';", rq->name);
		if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
			cout << "��ѯ��ʧ��" << szSql << endl;
			return;
			//��ѯ���ݿ�ʧ�ܵ��ĸ�ԭ��:1�����ݿ�û�н��� 2��δ���� 
			//3�����ӵĿ���û��Ҫ��ѯ��  4��sql������
		}
		//7����ѯ����Ƿ�Ϊ��
		if (0!=lstStr.size()) {
			//8�������ѯ���Ϊ��Ϊ�գ�˵���ѱ�ע��  _def_register_name_exit
			rs.result = _def_register_name_exist;
		}
		else {
			//9�������ѯ���Ϊ��Ϊ�գ�˵��δ��ע�ᣬ��ʼע��
			//��ע����Ϣ���浽���ݿ���
			sprintf_s(szSql,"insert into t_user (name,tel,password,iconId,feeling) values ('%s','%s','%s',8,'suisIneedu');" ,
				 rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(szSql)) {
				cout<<"�������ݿ�ʧ��:" <<szSql << endl;
				return;
			}
			//10��_def_register_success
			rs.result = _def_register_success;
		}
	}
	//11�����ͻ��˻�һ��ע��ظ�
	m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
}

//�����¼����
void CKernel::dealLoginRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2��У�������Ƿ�Ϸ�
	//����д��

	//3�������ֻ��Ų�ѯ����
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf_s(szSql, "select password,id from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(szSql, 2, lstStr)) {
		cout << "��ѯ��ʧ��" << szSql << endl;
		return;
	}
	//4���жϲ�ѯ����Ƿ�Ϊ��
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size()) {
		//5�����Ϊ�գ�˵��û��ע�ᣬ_def_login_tel_not_exist
		rs.result = _def_login_tel_not_exist;
	}
	else {
		//6����list��ȡ������
		string pass = lstStr.front();
		lstStr.pop_front();

		//��list��ȡ��id
		int userId = stoi(lstStr.front());
		lstStr.pop_front();
		 
		//7���ж����ݿ�����������������Ƿ����
		if (pass == rq->password) {
			//8�������� ��½�ɹ� _def_login_success
			rs.result = _def_login_success;
			rs.userId = userId;
			//��½�ɹ� ���ͻ��˻�һ����¼�ظ�
			m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
			//�����¼�û���id��socket
			m_mapIdToSocket[userId] = from;
			 
			//��ȡ��ǰ��¼�û�����Ϣ�Լ����ѵ���Ϣ
			getUserInfoAndFriendInfo(userId);

			//��ȡ��¼�û��ĺ���id�б�

			//��������id�б� �жϺ����Ƿ����ߣ�֪ͨ���ߵĺ����Լ�������


			return;
		}
		else {
			//9����¼ʧ�� ������� _def_login_password_error
			rs.result = _def_login_password_error;
		}
	}
	//���ܽ����Σ���Ҫ���ͻ��˻�һ����¼�ظ�
	m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::getUserInfoAndFriendInfo(int userId) {
	cout<<__func__<<endl;
	//1������id��ѯ�û���Ϣ
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);
    //2���ѵ�½����Ϣ�����ͻ���
	if (m_mapIdToSocket.count(userId)>0) {
		m_pNetMediator -> sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}
	//3�������û�id��ѯ������Ϣ
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql,"select idB from t_friend where idA = '%d';" ,userId);
	if (!m_sql.SelectMySql(szSql,1,lstStr)) {
		cout<<"��ѯ��ʧ��" << szSql << endl;
		return;
	}
	//��������id�б�
	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size()>0 ) {
		//ȡ�����ѵ�id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//���ݺ��ѵ�id��ѯ������Ϣ
		getInfoById(friendId, &friendInfo);

		//�Ѻ��ѵ���Ϣ�����ͻ���
		if (m_mapIdToSocket.count(userId) > 0) {
			m_pNetMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
		}
		//�жϺ����Ƿ����ߣ�֪ͨ���ߵĺ����Լ�������
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pNetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

//����id��ѯ��Ϣ
	//         ���������Ҫ��ѯ���û�    ������������ز�ѯ�����û���Ϣ
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info) {
	cout << __func__ << endl;
	//����id�����ݿ��в�ѯname ,feeling,iconid
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql, "select name,feeling,iconId from t_user where id = '%d';", id);
	if (!m_sql.SelectMySql(szSql, 3, lstStr)) {
		cout << "��ѯ��ʧ��" << szSql << endl;
		return;
	}
	//�жϲ�ѯ����Ƿ�������
	if (3 == lstStr.size()) {
		//ȡ��name,���浽�ṹ������
		strcpy_s(info->name,sizeof(info->name),lstStr.front().c_str());
		lstStr.pop_front();
		//ȡ��feeling,���浽�ṹ������
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();
		
		//ȡ��iconid,���浽�ṹ������
		info->iconId = stoi(lstStr.front());
		lstStr.pop_front();

	}
	else {
		cout<<"sql��ѯ�����������: "<<lstStr.size() << endl;
	}
	//�жϵ�ǰ�û��Ƿ�����
	if (m_mapIdToSocket.count(id) > 0) {
		info->status = _def_status_online;
	}
	else {
		info->status = _def_status_offline;
	}
	//��id��ֵ
	info->id = id;
}

//������������
void CKernel::dealOfflineRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2�����������û���Id��ѯ����id���б�
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql, "select idB from t_friend where idA = '%d';", rq->userId);
	if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
		cout << "��ѯ��ʧ��" << szSql << endl;
		return;
	}
	//3��������ѯ�Ľ��
	int friendId = 0;
	while (lstStr.size()>0) {
		//4��ȡ��ÿ�����ѵ�id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5���жϺ����Ƿ�����
		if (m_mapIdToSocket.count(friendId)>0) {
			//6�������ߵĺ���ת����������
			m_pNetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}

	//7����map��ɾ���û���socekt
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		SOCKET sock = m_mapIdToSocket[rq->userId];
		closesocket(sock);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//������������
void CKernel::dealChatRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;
	//2���жϺ����Ƿ�����
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//3���������� �Ͱ���������ת��������
		m_pNetMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//4�����Ѳ����ߣ��͸����ͷ����غ��Ѳ�����
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}
//������Ӻ�������
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2�����ߺ����ǳƲ�ѯ����id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf_s(szSql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
		cout << "��ѯ��ʧ��" << szSql << endl;
		return;
	}
	//3����ѯ����Ƿ�Ϊ��
	if (0 == lstStr.size()) {
		//4��û�˽���� ���ʧ�� _def_add_friend_not_exist
		_STRU_ADD_FRIEND_RS rs;
		rs.result = _def_add_friend_not_exist;
		rs.userId = rq->userId;
		strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
		//����ӽ�����ظ��ͻ���
		m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//5����ѯ�����Ϊ��
		int friendId = stoi(lstStr.front());
        lstStr.pop_front();
		//6���ж�����
		if (m_mapIdToSocket.count(friendId)>0) {
			//7�����ߣ�������Ӻ�������
			m_pNetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//8�������ߣ�������Ӻ�������ʧ��_def_add_friend_offline
			_STRU_ADD_FRIEND_RS rs;
			rs.result = _def_add_friend_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
			//����ӽ�����ظ��ͻ���
			m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}
//������Ӻ��ѻظ�
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1�����
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
	//2���ж���ӽ���Ƿ���ͬ��
	char szSql[1024] = "";
	if (_def_add_friend_success == rs->result) {
		//3���Ѻ��ѹ�ϵд�����ݿ�
		sprintf_s(szSql, "insert into t_friend values ('%d','%d');",rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(szSql)) {
			cout << "�������ݿ�ʧ��:" << szSql << endl;
			return;
		}
		sprintf_s(szSql, "insert into t_friend values ('%d','%d');",rs->userId,rs->friendId);
		if (!m_sql.UpdateMySql(szSql)) {
			cout << "�������ݿ�ʧ��:" << szSql << endl;
			return;
		}

		//4������˫�˺����б�
		getUserInfoAndFriendInfo(rs->userId);
	}
	//5���ѽ�����ظ��ͻ���
	if (m_mapIdToSocket.count(rs->friendId) > 0) {
		m_pNetMediator->sendData(data, len, m_mapIdToSocket[rs->friendId]);
	}
}