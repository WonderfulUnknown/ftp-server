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
//	// TODO: 在此添加专用代码和/或调用基类
//	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//主窗口指针对象
//	MySocket *sock = new MySocket();
//	if (Accept(*sock))
//	{
//		//msg = "220 ftp-server 192.168.100.99 is ready";
//		//sock->Send(msg, strlen(msg), 0);
//		log = L"UDP连接成功";
//		//触发FD_READ事件，调用OnReceive函数
//		AsyncSelect(FD_READ);
//	}
//	else
//	{
//		log = "UDP连接失败\r\n";
//		sock->Close();
//	}
//	dlg->m_Log.AddString(log);
//	CAsyncSocket::OnAccept(nErrorCode);
//}


//参数nErrorCode,函数调用时，MFC框架提供，表明套接字最新的状态
//如果是0，函数正常执行，没有错误；非0，套接字对象出错
void MySocket::OnSend(int nErrorCode)
{
	//绑定客户端的Ip和端口
	Connect(client_ip, 21);
	
	//Connect()
	Send(msg, strlen(msg), 0);
	//继续触发FD_READ事件,接收socket消息  
	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
}

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
	if (Quit)//退出
	{
		MySocket sock;
		sock.OnClose(0);
		Quit = false;
		return;
	}

	CAsyncSocket::OnReceive(nErrorCode);
}
