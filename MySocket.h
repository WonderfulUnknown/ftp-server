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

	char data[98000];//�������ݵĻ�����
	char *msg;//���͵�����
	CString log;//ÿ����������־��д������
	int length;//�������ݵĳ���
	CString receive;//��Ŵӻ�������ȡ����������

	bool Quit;//�Ƿ���ܵ�quit����
	bool IsError;//�ж������Ƿ��д�
};

