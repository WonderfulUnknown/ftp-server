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
	memset(data, 0, sizeof(data));
}


MySocket::~MySocket()
{
}

packet::packet()
{
	memset(data, 0, 1024);
	end = FALSE;
}

packet::~packet()
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
			user_name = receive.Mid(5);//��0��ʼ����
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
					name = name.Mid(9);
					SendFile(name);
				}
				else if (receive.Left(6) == "UPLOAD")
				{
					IsData = TRUE;
					CString name(receive);
					name = name.Mid(7);
					RecvFile(name);
				}
				else if (receive.Left(6) == "DELETE")
				{
					CString name(receive);
					name = name.Mid(7);
					DelFile(name);
					msg = "230 Delete successfully";
				}
				else if (receive.Left(4) == "QUIT")
					msg = "GoodBye!";
			}
			else
				msg = "500 Error: please login first";
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

void MySocket::SendFile(CString name)
{
	int num = 0;
	packet send, recv;

	CString filepath = user.GetFilePath(name);
	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
	file.SeekToBegin();
	send.length = file.Read(send.data, 1024);

	while (send.length)
	{
		send.number = num;
		SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
		Sleep(1);

		int length =  ReceiveFrom((char*)&recv, sizeof(recv), client_ip, client_port, 0);
		if (length != SOCKET_ERROR)
		{
			if (recv.number == num + 1)
			{
				num++;

				memset(send.data, 0, sizeof(send.data));
				send.length = file.Read(send.data, 1024);
			}
		}
	}
	send.end = true;
	//��������Ϊ�գ���־���������ݰ�
	SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
	file.Close();

	IsData = FALSE;
	msg = "";
	return;
}

void MySocket::RecvFile(CString name)
{
	int num = 0;
	packet send, recv;

	CString filepath = L"File\\" + name;
	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);

	//ֻ����ACK
	send.length = 3;
	strcpy_s(send.data, send.length + 1, "ACK");//��ȫ����
	
	while (1)
	{
		Sleep(1);
		ReceiveFrom((char*)&recv, sizeof(recv), client_ip, client_port, 0);

		if (recv.end == true)//�յ����һ�����ݰ�
			break;
		if (recv.number == num)
		{
			num++;

			file.Seek(0, CFile::end);
			file.Write(recv.data, recv.length);
		}
		//����յ������ݺ��벻�ԣ����ص�����number�����ظ�������һ�ε�ACK
		send.number = num;
		SendTo((char*)&send, sizeof(send), client_port, client_ip, 0);
	}
	file.Close();

	//�ļ��ϴ�����Ҫ����Ŀ¼
	user.GetList();
	IsData = FALSE;
	return;
}

void MySocket::DelFile(CString name)
{
	CString filepath = L"File\\" + name;
	CFile::Remove(filepath);

	user.GetList();
	return;
}