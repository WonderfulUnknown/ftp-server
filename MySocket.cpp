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
	IsLogin = FALSE;

	IsData = FALSE;
}


MySocket::~MySocket()
{
}

void MySocket::OnSend(int nErrorCode)
{
	SendTo(msg, strlen(msg), client_port, client_ip, 0);
	//��������FD_READ�¼�,����socket��Ϣ  
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}

//����nErrorCode,��������ʱ��MFC����ṩ�������׽������µ�״̬
//�����0����������ִ�У�û�д��󣻷�0���׽��ֶ������
void MySocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//������ָ�����
	//ÿ��receive֮ǰ��Ҫ�ѻ���������
	memset(data, 0, sizeof(data));
	//length�洢�����յ���Ϣ�ĳ��ȣ����յ������ݴ浽data��
	length = ReceiveFrom(data, sizeof(data), client_ip, client_port, 0);//�ͻ��˷��͵Ķ˿��������
	receive = data;
	if (!IsData)
	{
		//�����������
		if (!receive.Left(length).IsEmpty() && receive.Left(3) != "ACK")
		{
			log = L"C:" + receive.Left(length);
			dlg->m_Log.AddString(log);
		}
	}

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
			{
				msg = "230 USER login successfully";
				IsLogin = TRUE;
			}
			else
				msg = "500 Pwd is not correct";
		}
		else
		{
			if (IsLogin)
			{
				if (receive.Left(4) == "LIST")
				{
					dlg->m_FileList.ResetContent();//����б�
					user.filename = "";
					user.GetList();//��ȡ�ļ��б�
					USES_CONVERSION;
					msg = T2A(user.filename);
					CString List(msg);
					int index = List.Find(L",");
					while (index != -1)
					{
						dlg->m_FileList.AddString(List.Left(index));
						List = List.Right(List.GetLength() - index - 1);
						index = List.Find(L",");
					}
					dlg->m_FileList.AddString(List);
				}
				else if (receive.Left(8) == "DOWNLOAD")
				{
					IsData = TRUE;
					CString name(receive);
					name = name.Right(name.GetLength() - name.Find(L":") - 1);
					SendFile(name);
				}
				else if (receive.Left(6) == "UPLOAD")
				{
					IsData = TRUE;
					CString name(receive);
					name = name.Right(name.GetLength() - name.Find(L":") - 1);
					RecvFile(name);
				}
				else if (receive.Left(4) == "QUIT")
					msg = "GoodBye!";
			}
			else
				msg = "500 Error: login fail";
		}

		if (!IsData)
		{
			if (strlen(msg) != 0 && msg != "ACK")
			{
				SendTo(msg, strlen(msg), client_port, client_ip, 0);
				log = L"S:" + (CString)msg;
				dlg->m_Log.AddString(log);
			}
		}

		//AsyncSelect(FD_WRITE);
	}
	CAsyncSocket::OnReceive(nErrorCode);
}

//���ļ����ݶ�ȡ������
void MySocket::SendFile(CString name)
{
	MySocket socket;
	CString filepath = user.GetFilePath(name);
	packet send;
	packet recv;
	int num = 0;
	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
	file.SeekToBegin();
	send.length = file.Read(send.data, 1024);

	send.end = false;
	while (send.length)
	{
		send.number = num;;
		msg = (char *)&send;
		SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
		Sleep(10);
		
		length = ReceiveFrom((char*)&recv, sizeof(recv), client_ip, client_port, 0);
		if (recv.number == num + 1)         //��������������ݰ������ոշ��͵����ݰ�û�д���
		{
			num++;
			memset(send.data, 0, sizeof(send.data));
			send.length = file.Read(send.data, 1024);
		}
	}
	send.end = true;
	IsData = FALSE;
	SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
	file.Close();
}

// �������ݲ�д���ļ�
void MySocket::RecvFile(CString name)
{
	MySocket socket;
	CString filepath = L"File\\" + name;
	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	int num = 0;
	packet send;
	packet recv;
	send.end = false;
	while (1)
	{
		ReceiveFrom((char*)&recv, sizeof(recv), client_ip, client_port, 0);
		if (recv.end == true)//�ļ��������
			break;
		else
		{
			if (recv.number == num)
			{
				recv.data[recv.length] = '\0';
				file.SeekToEnd();
				file.Write(recv.data, recv.length);
				num++;
			}
			strcpy(send.data, "ACK");
			send.length = strlen(send.data);
			send.number = num;
			SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
		}
	}
	file.Close();
	//�ļ��ϴ�����Ҫ����Ŀ¼
	user.GetList();
	return;
}