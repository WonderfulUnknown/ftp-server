#include "stdafx.h"
#include "MySocket.h"
#include "ftp_server.h"
#include "ftp_serverDlg.h"

using namespace std;

MySocket::MySocket()
{
	Quit = false;
}


MySocket::~MySocket()
{
}


//����nErrorCode,��������ʱ��MFC����ṩ�������׽������µ�״̬
//�����0����������ִ�У�û�д��󣻷�0���׽��ֶ������
void MySocket::OnAccept(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//������ָ�����
	MySocket *sock = new MySocket();
	if (Accept(*sock))
	{
		//msg = "220 ftp-server 192.168.100.99 is ready";
		//sock->Send(msg, strlen(msg), 0);
		log = L"TCP���ӳɹ�";
		//����FD_READ�¼�������OnReceive����
		sock->AsyncSelect(FD_READ);
	}
	else
	{
		log = "TCP����ʧ��\r\n";
		sock->Close();
	}
	dlg->m_Log.AddString(log);
	CAsyncSocket::OnAccept(nErrorCode);
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

	//�����ݴӵȴ����ж��뻺���������Ҳ������ݴӻ��������
	//length = Receive(data, sizeof(data), MSG_PEEK);
	//�����������
	//length = Receive(data, sizeof(data), MSG_OOB);
	receive = data;
	log = L"C:" + receive.Left(length);
	dlg->m_FileList.AddString(log);
	

	if (length != SOCKET_ERROR)
	{
		if (receive.Left(4) == "USER" && receive.Mid(5) == "admin")//receive.Right(5)
			msg = "331 USER command is OK,require PASS";
		else if (receive.Left(4) == "PASS")
			msg = "230 USER log in successfully";
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
	log = L"S:" + (CString)msg;
	dlg->m_FileList.AddString(log);
	if (Quit)//�˳�
	{
		MySocket sock;
		sock.OnClose(0);
		Quit = false;
		return;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}
