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
	//继续触发FD_READ事件,接收socket消息  
	AsyncSelect(FD_READ);
	//while(AsyncSelect(FD_READ) != 0)
	//while (!AsyncSelect(FD_READ))
		//Sleep(10);
	CAsyncSocket::OnSend(nErrorCode);
}

//参数nErrorCode,函数调用时，MFC框架提供，表明套接字最新的状态
//如果是0，函数正常执行，没有错误；非0，套接字对象出错
void MySocket::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);
}

void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//主窗口指针对象
	//每次receive之前需要把缓冲区清零
	memset(data, 0, sizeof(data));
	//length存储返回收到消息的长度，接收到的数据存到data中
	length = ReceiveFrom(data, sizeof(data), client_ip, client_port, 0);//客户端发送的端口是随机的

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
	if (Quit)//退出
	{
		MySocket sock;
		sock.OnClose(0);
		Quit = false;
		return;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}

// 向客户端发送目录
void MySocket::SendList()
{
	USES_CONVERSION;
	char* filelist = T2A(user.filename);
	SendTo(filelist, strlen(filelist), client_port, client_ip, 0);
}