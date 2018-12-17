#include "stdafx.h"
#include "ftp_server.h"
#include "ftp_serverDlg.h"
#include "User.h"

using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS
User user;
MySocket::MySocket()
{
	Quit = false;
	If_Data = false;
}


MySocket::~MySocket()
{
}


//void MySocket::OnAccept(int nErrorCode)
//{
//	// TODO: �ڴ����ר�ô����/����û���
//	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//������ָ�����
//	MySocket *sock = new MySocket();
//	if (Accept(*sock))
//	{
//		//msg = "220 ftp-server 192.168.100.99 is ready";
//		//sock->Send(msg, strlen(msg), 0);
//		log = L"UDP���ӳɹ�";
//		//����FD_READ�¼�������OnReceive����
//		AsyncSelect(FD_READ);
//	}
//	else
//	{
//		log = "UDP����ʧ��\r\n";
//		sock->Close();
//	}
//	dlg->m_Log.AddString(log);
//	CAsyncSocket::OnAccept(nErrorCode);
//}


//����nErrorCode,��������ʱ��MFC����ṩ�������׽������µ�״̬
//�����0����������ִ�У�û�д��󣻷�0���׽��ֶ������
void MySocket::OnSend(int nErrorCode)
{
	//�󶨿ͻ��˵�Ip�Ͷ˿�
	Connect(client_ip, 21);
	
	//Connect()
	Send(msg, strlen(msg), 0);
	//��������FD_READ�¼�,����socket��Ϣ  
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}

void MySocket::OnClose(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnClose(nErrorCode);
}

void MySocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//������ָ�����
	//ÿ��receive֮ǰ��Ҫ�ѻ���������
	memset(data, 0, sizeof(data));
	//length�洢�����յ���Ϣ�ĳ��ȣ����յ������ݴ浽data��
	length = Receive(data, sizeof(data), 0);
	
	int addrlen = sizeof(client_addr);
	//length = recvfrom((SOCKET)this, data, sizeof(data), 0, (SOCKADDR*)&client_addr, &addrlen);
	//int error;
	//error = WSAGetLastError();
	//client_ip = inet_ntoa(client_addr.sin_addr);
	//AfxMessageBox(client_ip, MB_ICONINFORMATION);

	GetSockName((SOCKADDR*)&client_addr, &addrlen);
	client_ip = inet_ntoa(client_addr.sin_addr);
	AfxMessageBox(client_ip, MB_ICONINFORMATION);

	//GetPeerName((SOCKADDR*)&client_addr, &addrlen);
	//client_ip = inet_ntoa(client_addr.sin_addr);
	//AfxMessageBox(client_ip, MB_ICONINFORMATION);
	////getpeername((SOCKET)this,(struct sockaddr *)&client_addr, &addrlen);

	////int error;
	////if(getsockname((SOCKET)this, (struct sockaddr *)&client_addr, &addrlen)== SOCKET_ERROR)
	////	error = WSAGetLastError();
	////AfxMessageBox(client_ip, MB_ICONINFORMATION);

	receive = data;
	log = L"C:" + receive.Left(length);
	dlg->m_Log.AddString(log);
	
	if (length != SOCKET_ERROR)
	{
		if (receive.Left(4) == "USER")
		{
			user_name = receive.Mid(5);
			if (user.CheckUser(user_name).IsEmpty())
				msg = "500 USER is not exist";
			else 
				msg = "331 USER command is OK,require PASS";
		}
		else if (receive.Left(4) == "PASS")
		{
			CString pwd = receive.Mid(5);
			if (pwd == user.CheckUser(user_name))
				msg = "230 USER log in successfully";
			else
				msg = "500 Pwd is not correct";
		}
		else if (receive.Left(4) == "QUIT")
		{
			msg = "GoodBye!";
			Quit = TRUE;
		}
		else
			msg = "500 Error: bad syntax";
		//else if (receive.Left(3) == "PWD")
		//else if (receive.Left(4) == "PASV")
		//els if (receive.Left(4) == "TYPE")
		//else if (receive.Left(4) == "LIST")
	}
	else
		msg = "500 Error: bad syntax";

	AsyncSelect(FD_WRITE);
	log = L"S:" + (CString)msg;
	dlg->m_Log.AddString(log);
	if (Quit)//�˳�
	{
		MySocket sock;
		sock.OnClose(0);
		Quit = false;
		return;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}
