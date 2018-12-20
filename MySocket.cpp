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
	//继续触发FD_READ事件,接收socket消息  
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}

//参数nErrorCode,函数调用时，MFC框架提供，表明套接字最新的状态
//如果是0，函数正常执行，没有错误；非0，套接字对象出错
void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//主窗口指针对象
	//每次receive之前需要把缓冲区清零
	memset(data, 0, sizeof(data));
	//length存储返回收到消息的长度，接收到的数据存到data中
	length = ReceiveFrom(data, sizeof(data), client_ip, client_port, 0);//客户端发送的端口是随机的
	receive = data;
	if (!IsData)
	{
		//避免冗余输出
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
					dlg->m_FileList.ResetContent();//清空列表
					user.filename = "";
					user.GetList();//获取文件列表
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

//将文件内容读取并发送
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
		if (recv.number == num + 1)         //如果发回来的数据包表明刚刚发送的数据包没有错误
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

// 接收数据并写入文件
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
		if (recv.end == true)//文件接收完成
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
	//文件上传后需要更新目录
	user.GetList();
	return;
}