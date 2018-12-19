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
				else if (receive.Left(4) == "QUIT")
					msg = "GoodBye!";
				//else if (receive.Left(3) == "PWD")
				//else if (receive.Left(4) == "PASV")
				//els if (receive.Left(4) == "TYPE")
			}
			else
				msg = "500 Error: login fail";
		}
	}
	/*else
		msg = "500 Error: bad syntax";*/

	AsyncSelect(FD_WRITE);
	log = L"S:" + (CString)msg;
	dlg->m_Log.AddString(log);
	CAsyncSocket::OnReceive(nErrorCode);
}

//// ��ͻ��˷���Ŀ¼
//void MySocket::SendList()
//{
//	USES_CONVERSION;
//	char* filelist = T2A(user.filename);
//	SendTo(filelist, strlen(filelist), client_port, client_ip, 0);
//}

//���ļ����ݶ�ȡ������
void MySocket::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
{
	int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	CString filepath = user.GetFilePath(name);
	packet send;   //Ҫ���͵Ľṹ
	packet recv;
	int num = 0;  //���͵ڼ������ݰ�
	int len = sizeof(addr_aim);
	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
	file.SeekToBegin();
	int length_packet = file.GetLength();
	send.length = file.Read(send.data, 1024);
	send.end = false;
	while (send.length)
	{
		send.number = num;
		sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
		if (recv.number == num + 1)         //��������������ݰ������ոշ��͵����ݰ�û�д���
		{
			num++;
			send.length = file.Read(send.data, 1024);
		}
	}
	send.end = true;
	sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
	file.Close();
	timeout = 10000; //ƽʱ����ʱ��Ϊ10s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	return;
}

// �������ݲ�д���ļ�
void MySocket::RecvData(CString name, sockaddr_in addr_aim, SOCKET server)
{
	int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	//restart_server(server);
	CString filepath = L"list\\" + name;
	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);       //�������Զ����ƴ�Ҫ���ص��ļ�
	int len = sizeof(addr_aim);
	int num = 0;  //��һ��Ҫ���ܵ����ݰ�id
	bool end = false;
	packet send;
	packet recv;
	send.end = false;
	while (!end)
	{
		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
		if (recv.end == true)   //�ļ��������
		{
			end = true;
		}
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
			sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
		}
	}
	file.Close();
	timeout = 10000; //ƽʱ����ʱ��Ϊ10s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	user.GetList();        //�����ϴ��ļ������»�ȡĿ¼
	return;
}