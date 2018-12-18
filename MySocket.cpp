#include "stdafx.h"
#include "ftp_server.h"
#include "ftp_serverDlg.h"
#include "User.h"
#include <atlconv.h>

using namespace std;
#define _WINSOCK_DEPRECATED_NO_WARNINGS
User user;
MySocket::MySocket()
{
	Quit = false;
}


MySocket::~MySocket()
{
}

void MySocket::OnSend(int nErrorCode)
{
	//AfxMessageBox(client_ip, MB_ICONINFORMATION);
	//CString temp;
	//temp.Format(_T("%d"), client_port);
	//AfxMessageBox(temp, MB_ICONINFORMATION);
	SendTo(msg, strlen(msg), client_port, client_ip, 0);
	//��������FD_READ�¼�,����socket��Ϣ  
	AsyncSelect(FD_READ);
	//while(AsyncSelect(FD_READ) != 0)
	//while (!AsyncSelect(FD_READ))
		//Sleep(10);
	CAsyncSocket::OnSend(nErrorCode);
}

//����nErrorCode,��������ʱ��MFC����ṩ�������׽������µ�״̬
//�����0����������ִ�У�û�д��󣻷�0���׽��ֶ������
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
	length = ReceiveFrom(data, sizeof(data), client_ip, client_port, 0);//�ͻ��˷��͵Ķ˿��������

	receive = data;
	//AfxMessageBox(receive, MB_ICONINFORMATION);

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
				msg = "230 USER login successfully";
			else
				msg = "500 Pwd is not correct";
		}
		else if (receive.Left(4) == "QUIT")
		{
			msg = "GoodBye!";
			Quit = TRUE;
		}
		else if (receive.Left(4) == "LIST")
			SendList();
		else
			msg = "500 Error: bad syntax";
		//else if (receive.Left(3) == "PWD")
		//else if (receive.Left(4) == "PASV")
		//els if (receive.Left(4) == "TYPE")
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

// ��ͻ��˷���Ŀ¼
void MySocket::SendList()
{
	USES_CONVERSION;
	char* filelist = T2A(user.filename);
	SendTo(filelist, strlen(filelist), client_port, client_ip, 0);
}