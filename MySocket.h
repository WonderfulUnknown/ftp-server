#pragma once
#include "afxsock.h"
//#include "User.h"
class MySocket :public CAsyncSocket
{
public:
	MySocket();
	~MySocket();

	//void OnAccept(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);
	
//	User user;
	CString user_name;
	char data[98000];//接受数据的缓冲区
	char *msg;//发送的数据
	CString log;//每次往交互日志中写的内容
	int length;//返回数据的长度
	CString receive;//存放从缓冲区中取出来的数据
	CString client_ip;//存储客户端地址
	SOCKADDR_IN client_addr;//存储客户端地址
	UINT client_port;//客户端端口
	int addrlen;

	bool Quit;//是否接受到quit命令
	bool IsError;//判断命令是否有错
};

