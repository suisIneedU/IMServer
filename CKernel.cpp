#include "CKernel.h"
#include"mediator/TcpServerMediator.h"

//静态成员变量类外初始化
CKernel* CKernel::pKernel =nullptr;

CKernel::CKernel():m_pNetMediator(nullptr){
	pKernel = this;
	setProtocolFun();
}

//初始化函数指针数组
void CKernel::setProtocolFun(){
	cout<<__func__<<endl;

	//数组初始化
	memset(m_vectorProtocolFun,0,sizeof(m_vectorProtocolFun));

	//把函数地址保存到数组中
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

//打开服务端
bool CKernel::startServer(){

	//打开网络
	m_pNetMediator = new TcpServerMediator;
	if (!m_pNetMediator->openNet()) {
		cout << "打开服务端网络失败" << endl;
		return false;
	}
	//TODO :连接数据库
	char ip[] = "127.0.0.1";
	char name[] = "root";
	char pass[] = "123456";
	char dbName[] = "im";
	if (!m_sql.ConnectMySql(ip, name, pass, dbName)){
		cout<<"连接数据库失败" << endl;
		return false;
	}
	else {
		cout << "连接数据库成功" << endl;
	}
	return true;
}
//关闭服务端
void CKernel::closeServer(){
	if (m_pNetMediator) {
		m_pNetMediator->closeNet();
		delete m_pNetMediator;
		m_pNetMediator = nullptr;
	}
}

//处理、分发所有接收到的数据
void CKernel::dealData(char* data, int len, unsigned long from) {
	cout<<__func__<<endl;
	//取出type的值
	int type = *(int*)data;
	//根据type的值，判断是哪个结构体,调用不同的处理函数
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
	//根据type值计算数组下标
	int index = type - _PROTOCOL_BASE - 1;

	//判断数组下标是否越界
	if (index>= 0 && index < _DEF_PROTOCOL_COUNT) {
		//根据数组下标取出函数地址
		PFUN pf = m_vectorProtocolFun[index];
		//判断函数地址是否为空，不为空就调用对应的处理函数
		if (pf) {
			(this->*pf)(data, len, from);
		}
		else {
			cout<< "函数地址为空,type:" <<type<< endl;
		}
	}
	else {
		//type值特别大:1、offset没清零 2、结构体定义的时候没赋值 3、定义的时候没放在最开头
		cout << "type:" << type << endl;
	}
}

//处理注册请求
void CKernel::dealRegisterRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	
	//2、校验数据是否合法

	//(1)判断字符串是否为空或者是全空格
	//if (name.isEmpty() || tel.isEmpty() || password.isEmpty() ||
	//	nameTmp.remove(" ").isEmpty() || telTmp.remove(" ").isEmpty() || passwordTmp.remove(" ").isEmpty()) {                                     //remove()会改变数据的原始内容
	//	QMessageBox::about(this, "提示", "不能输入空的内容或者全空格！");
	//	return;
	//}
	//(2)判断字符串长度是否合法
	/*if (name.length() > 20 || password.length() > 20 || 11 != tel.length()) {
		return;
	}*/

	//3、根据电话号码从数据库中查询
	list<string> lstStr;
	char szSql[1024] ="";
	sprintf_s(szSql, "select tel from t_user where tel ='%s';",rq->tel);
	if (!m_sql.SelectMySql(szSql,1,lstStr)) {
		//查询数据库失败的四个原因:1、数据库没有建立  2、未连接 
		//3、连接的库中没有要查询表  4、sql语句错误
		cout<<"查询库失败" << szSql<< endl;
		return;
	}

	_STRU_REGISTER_RS rs;
	// 4、判断查询结果是否为空
	if (0 !=lstStr.size()) {
		//5、如果查询结果为不为空，说明已被注册 _def_register_tel_exit
		rs.result = _def_register_tel_exist;
	}
	else {
		//6、如果查询结果为空，说明未被注册
		//根据昵称从数据库中查询
		sprintf_s(szSql, "select name from t_user where name ='%s';", rq->name);
		if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
			cout << "查询库失败" << szSql << endl;
			return;
			//查询数据库失败的四个原因:1、数据库没有建立 2、未连接 
			//3、连接的库中没有要查询表  4、sql语句错误
		}
		//7、查询结果是否为空
		if (0!=lstStr.size()) {
			//8、如果查询结果为不为空，说明已被注册  _def_register_name_exit
			rs.result = _def_register_name_exist;
		}
		else {
			//9、如果查询结果为不为空，说明未被注册，开始注册
			//把注册信息保存到数据库中
			sprintf_s(szSql,"insert into t_user (name,tel,password,iconId,feeling) values ('%s','%s','%s',8,'suisIneedu');" ,
				 rq->name,rq->tel,rq->password);
			if (!m_sql.UpdateMySql(szSql)) {
				cout<<"插入数据库失败:" <<szSql << endl;
				return;
			}
			//10、_def_register_success
			rs.result = _def_register_success;
		}
	}
	//11、给客户端回一个注册回复
	m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
}

//处理登录请求
void CKernel::dealLoginRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	
	//2、校验数据是否合法
	//懒得写了

	//3、根据手机号查询密码
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf_s(szSql, "select password,id from t_user where tel = '%s';", rq->tel);
	if (!m_sql.SelectMySql(szSql, 2, lstStr)) {
		cout << "查询库失败" << szSql << endl;
		return;
	}
	//4、判断查询结果是否为空
	_STRU_LOGIN_RS rs;
	if (0 == lstStr.size()) {
		//5、如果为空，说明没有注册，_def_login_tel_not_exist
		rs.result = _def_login_tel_not_exist;
	}
	else {
		//6、从list中取出密码
		string pass = lstStr.front();
		lstStr.pop_front();

		//从list中取出id
		int userId = stoi(lstStr.front());
		lstStr.pop_front();
		 
		//7、判断数据库的密码和输入的密码是否相等
		if (pass == rq->password) {
			//8、如果相等 登陆成功 _def_login_success
			rs.result = _def_login_success;
			rs.userId = userId;
			//登陆成功 给客户端回一个登录回复
			m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
			//保存登录用户的id和socket
			m_mapIdToSocket[userId] = from;
			 
			//获取当前登录用户的信息以及好友的信息
			getUserInfoAndFriendInfo(userId);

			//获取登录用户的好友id列表

			//遍历好友id列表 判断好友是否在线，通知在线的好友自己上线了


			return;
		}
		else {
			//9、登录失败 密码错误 _def_login_password_error
			rs.result = _def_login_password_error;
		}
	}
	//不管结果如何，都要给客户端回一个登录回复
	m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::getUserInfoAndFriendInfo(int userId) {
	cout<<__func__<<endl;
	//1、根据id查询用户信息
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);
    //2、把登陆的信息发给客户端
	if (m_mapIdToSocket.count(userId)>0) {
		m_pNetMediator -> sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[userId]);
	}
	//3、根据用户id查询好友信息
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql,"select idB from t_friend where idA = '%d';" ,userId);
	if (!m_sql.SelectMySql(szSql,1,lstStr)) {
		cout<<"查询库失败" << szSql << endl;
		return;
	}
	//遍历好友id列表
	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size()>0 ) {
		//取出好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();

		//根据好友的id查询好友信息
		getInfoById(friendId, &friendInfo);

		//把好友的信息发给客户端
		if (m_mapIdToSocket.count(userId) > 0) {
			m_pNetMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdToSocket[userId]);
		}
		//判断好友是否在线，通知在线的好友自己上线了
		if (m_mapIdToSocket.count(friendId) > 0) {
			m_pNetMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdToSocket[friendId]);
		}
	}
}

//根据id查询信息
	//         输入参数，要查询的用户    输出参数，返回查询到的用户信息
void CKernel::getInfoById(int id, _STRU_FRIEND_INFO* info) {
	cout << __func__ << endl;
	//根据id从数据库中查询name ,feeling,iconid
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql, "select name,feeling,iconId from t_user where id = '%d';", id);
	if (!m_sql.SelectMySql(szSql, 3, lstStr)) {
		cout << "查询库失败" << szSql << endl;
		return;
	}
	//判断查询结果是否是三个
	if (3 == lstStr.size()) {
		//取出name,保存到结构体里面
		strcpy_s(info->name,sizeof(info->name),lstStr.front().c_str());
		lstStr.pop_front();
		//取出feeling,保存到结构体里面
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();
		
		//取出iconid,保存到结构体里面
		info->iconId = stoi(lstStr.front());
		lstStr.pop_front();

	}
	else {
		cout<<"sql查询结果个数错误: "<<lstStr.size() << endl;
	}
	//判断当前用户是否在线
	if (m_mapIdToSocket.count(id) > 0) {
		info->status = _def_status_online;
	}
	else {
		info->status = _def_status_offline;
	}
	//给id赋值
	info->id = id;
}

//处理下线请求
void CKernel::dealOfflineRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;

	//2、根据下线用户的Id查询好友id的列表
	list<string> lstStr;
	char szSql[1024];
	sprintf_s(szSql, "select idB from t_friend where idA = '%d';", rq->userId);
	if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
		cout << "查询库失败" << szSql << endl;
		return;
	}
	//3、遍历查询的结果
	int friendId = 0;
	while (lstStr.size()>0) {
		//4、取出每个好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5、判断好友是否在线
		if (m_mapIdToSocket.count(friendId)>0) {
			//6、给在线的好友转发下线请求
			m_pNetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
	}

	//7、从map里删除用户的socekt
	if (m_mapIdToSocket.count(rq->userId) > 0) {
		SOCKET sock = m_mapIdToSocket[rq->userId];
		closesocket(sock);
		m_mapIdToSocket.erase(rq->userId);
	}
}

//处理聊天请求
void CKernel::dealChatRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;
	//2、判断好友是否在线
	if (m_mapIdToSocket.count(rq->friendId) > 0) {
		//3、好友在线 就把聊天请求转发给好友
		m_pNetMediator->sendData(data, len, m_mapIdToSocket[rq->friendId]);
	}
	else {
		//4、好友不在线，就给发送方返回好友不在线
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}
//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;

	//2、更具好友昵称查询好友id
	list<string> lstStr;
	char szSql[1024] = "";
	sprintf_s(szSql, "select id from t_user where name = '%s';", rq->friendName);
	if (!m_sql.SelectMySql(szSql, 1, lstStr)) {
		cout << "查询库失败" << szSql << endl;
		return;
	}
	//3、查询结果是否为空
	if (0 == lstStr.size()) {
		//4、没人叫这个 添加失败 _def_add_friend_not_exist
		_STRU_ADD_FRIEND_RS rs;
		rs.result = _def_add_friend_not_exist;
		rs.userId = rq->userId;
		strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
		//把添加结果返回给客户端
		m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
	}
	else {
		//5、查询结果不为空
		int friendId = stoi(lstStr.front());
        lstStr.pop_front();
		//6、判断在线
		if (m_mapIdToSocket.count(friendId)>0) {
			//7、在线，发送添加好友请求
			m_pNetMediator->sendData(data, len, m_mapIdToSocket[friendId]);
		}
		else {
			//8、不在线，发送添加好友请求失败_def_add_friend_offline
			_STRU_ADD_FRIEND_RS rs;
			rs.result = _def_add_friend_offline;
			rs.userId = rq->userId;
			strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
			//把添加结果返回给客户端
			m_pNetMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}
//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from) {
	cout << __func__ << endl;
	//1、拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
	//2、判断添加结果是否是同意
	char szSql[1024] = "";
	if (_def_add_friend_success == rs->result) {
		//3、把好友关系写入数据库
		sprintf_s(szSql, "insert into t_friend values ('%d','%d');",rs->friendId, rs->userId);
		if (!m_sql.UpdateMySql(szSql)) {
			cout << "插入数据库失败:" << szSql << endl;
			return;
		}
		sprintf_s(szSql, "insert into t_friend values ('%d','%d');",rs->userId,rs->friendId);
		if (!m_sql.UpdateMySql(szSql)) {
			cout << "插入数据库失败:" << szSql << endl;
			return;
		}

		//4、更新双端好友列表
		getUserInfoAndFriendInfo(rs->userId);
	}
	//5、把结果发回给客户端
	if (m_mapIdToSocket.count(rs->friendId) > 0) {
		m_pNetMediator->sendData(data, len, m_mapIdToSocket[rs->friendId]);
	}
}