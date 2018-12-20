#pragma once
#include "afxsock.h"
class MySocket :public CAsyncSocket
{
public:
	MySocket();
	~MySocket();

	//void OnClose(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnReceive(int nErrorCode);

	//发送文件
	void SendFile(CString name);
	// 接收文件
	void RecvFile(CString name);

	CStdioFile file;

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

	bool IsData;//判断是否在传输数据
	bool IsLogin;//判断是否登录成功
};


//发送文件时的数据结构
struct packet
{
	char data[2048];//数据
	int number;//包的号码
	int length;//数据长度
	bool end;//代表结束的数据包
};