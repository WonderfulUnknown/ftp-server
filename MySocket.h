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
	char data[98000];//�������ݵĻ�����
	char *msg;//���͵�����
	CString log;//ÿ����������־��д������
	int length;//�������ݵĳ���
	CString receive;//��Ŵӻ�������ȡ����������
	CString client_ip;//�洢�ͻ��˵�ַ
	SOCKADDR_IN client_addr;//�洢�ͻ��˵�ַ
	UINT client_port;//�ͻ��˶˿�
	int addrlen;

	bool Quit;//�Ƿ���ܵ�quit����
	bool IsError;//�ж������Ƿ��д�
};

