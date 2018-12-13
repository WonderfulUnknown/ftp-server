#pragma once
#include "afxsock.h"
class MySocket :public CAsyncSocket
{
public:
	MySocket();
	~MySocket();

	void OnAccept(int nErrorCode);
	void OnClose(int nErrorCode);
	void OnReceive(int nErrorCode);

	//char data[98000];//接受数据的缓冲区
	//char *msg;//发送的数据
	////CString log;//每次往交互日志中写的内容
	//int length;//返回数据的长度
	//CString receive;//存放从缓冲区中取出来的数据
	//int step;//用于检验smtp协议是否按顺序
	//CString content;//用于存储邮件正文的base64码

	//bool IsData;//传输的是否是数据
	//bool Quit;//是否接受到quit命令
	//bool IsBmp;//附件是否有图片
	//bool IsError;//判断命令是否有错
};

