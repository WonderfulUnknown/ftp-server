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

	//// ��ͻ��˷���Ŀ¼
	//void SendList();
	// ���ļ����ݶ�ȡ������
	void SendData(CString name,sockaddr_in addr_aim,SOCKET server);
	// �������ݲ�д���ļ�
	void RecvData(CString name, sockaddr_in addr_aim, SOCKET server);

	CStdioFile file;

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

	bool IsError;//�ж������Ƿ��д�
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