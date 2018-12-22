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

	void SendFile(CString name);
	void RecvFile(CString name);
	void DelFile(CString name);
	
	CStdioFile file;

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

	bool IsData;//�ж��Ƿ��ڴ�������
	bool IsLogin;//�ж��Ƿ��¼�ɹ�
};


//�����ļ�ʱ�����ݽṹ
class packet
{
public:
	packet();
	~packet();
	
	char data[1024];//����
	int number;//���ݰ��ĺ���
	int length;//���ݳ���
	bool end;//������������ݰ�
};