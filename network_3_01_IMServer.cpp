#include <iostream>
#include<windows.h>
#include"CKernel.h"
using namespace std;

int main() {
	CKernel kernel;
	//打开服务器
	if (!kernel.startServer()) {
		cout<<"打开服务器失败" << endl;
		return 1;
	}
	while (1) {
		cout<<"server is running "<<endl;
		Sleep(5000);
	}

	return 0;
}

//聊天软件IM功能:
		// 注册
		// 登录 
		// 添加好友
		// 显示好友列表
		// 聊天
		// 下线

		//数据库设计
		//用户表:用户id(主键)\昵称、电话号、密码、头像编号、签名
		//好友关系表:用户id1、用户id2(双向存储)


		// C++面向对象编程
		// 服务端vS:
		// 数据库(保存注册用户的信息和好友关系)
		// 核心处理类(处理接收到的数据、组织要发送的数据)
		// 中介者类
		// 网络类(接收数据、发送数据、初始化网络、关闭网络)

		// 客户端QT:
		// UI界面类
		// 核心处理类(处理接收到的数据、组织要发送的数据)
		// 中介者类
		// 网络类(接收数据、发送数据、初始化网络、关闭网络)

		// 网络类(兼容UDP和TCP协议)--多态
		// INet父类-纯虚函数。
		// udp子类
		// TcpServer子类
		// TcpClient子类


		//解决头文件循环包含的方法：
		// 采用先声明后定义的方法:不通过include包含

		//两个类循环new对象

//总结:
//一:项目中存在的问题
//1、同一个账户可以同时登录多个客户端，但是只有最后一个客户端能收到消息--登录的时候判断当前用户是否在线
//2、手机号没有校验真实性--验证码
//3、异常下线(断网、电脑蓝屏。。。)，服务端感知不到--心跳机制
//4、windows操作系统给每个进程分配4G的虚拟内存，0-2G是内核空间，2-4G是用户空间
//当前项目使用的是10阻塞多线程模型，有多少个客户端就会创建多少个线程，每个线程的堆栈大小是1M
//总的空间是2G，最多能创建2千多个线程，也就允许2千个用户同时在线-一服务器改到1inux下使用epo11模型

//二、扩展功能
//1、发送文件
//2、表情包、朋友圈
//3、群组(创建群组、添加群成员、群聊)
//4、多人游戏
//5、新闻推送(学了爬虫以后写)
//6、不在线用户的聊天，不在线用户添加好友--完善
//7、聊天记录保存(本地保存、数据库中保存)