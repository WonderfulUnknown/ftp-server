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

	//�����ļ�
	void SendFile(CString name);
	// �����ļ�
	void RecvFile(CString name);

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
struct packet
{
	char data[2048];//����
	int number;//���ĺ���
	int length;//���ݳ���
	bool end;//������������ݰ�
};