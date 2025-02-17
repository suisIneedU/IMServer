#pragma once
#include<map>
#include<iostream>
#include"MySQL/CMySql.h"
#include"mediator/INetMediator.h"
#include"net/def.h"
using namespace std;

class CKernel;
//定义函数指针类型
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel
{
public:
	CKernel();
	~CKernel();

	//打开服务端
	bool startServer();
	//关闭服务端
	void closeServer();

	//初始化函数指针数组
	void setProtocolFun();
	
	//获取当前登录用户的信息以及好友的信息
	void getUserInfoAndFriendInfo(int userId);

	//根据id查询信息
	//         输入参数，要查询的用户    输出参数，返回查询到的用户信息
	void getInfoById(int id,_STRU_FRIEND_INFO* info);	

	//处理、分发所有接收到的数据
	void dealData(char* data, int len, unsigned long from);

	//处理登录请求
	void dealLoginRq(char* data, int len, unsigned long from);

	//处理下线请求
	void dealOfflineRq(char* data, int len, unsigned long from);

	//处理注册请求
	void dealRegisterRq(char* data, int len, unsigned long from);

	//处理聊天请求
	void dealChatRq(char* data, int len, unsigned long from);

	//处理添加好友请求
	void dealAddFriendRq(char* data, int len, unsigned long from);

	//处理添加好友回复
	void dealAddFriendRs(char* data, int len, unsigned long from);

	static CKernel* pKernel;
    public:
private:
	INetMediator* m_pNetMediator;
    CMySql m_sql;
	//函数指针数组
	PFUN m_vectorProtocolFun[_DEF_PROTOCOL_COUNT];
	//保存登录用户的id和socket(登陆成功的时候保存，下线的时候删除，
	//判断用户是否在线就可以判断map中是否有这个用户的id)
	map<int,SOCKET> m_mapIdToSocket;
};

