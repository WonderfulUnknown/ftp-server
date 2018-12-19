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

//// 向客户端发送目录
//void MySocket::SendList()
//{
//	USES_CONVERSION;
//	char* filelist = T2A(user.filename);
//	SendTo(filelist, strlen(filelist), client_port, client_ip, 0);
//}

//将文件内容读取并发送
void MySocket::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
{
	int timeout = 2000;      //设置超时时间为2s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	CString filepath = user.GetFilePath(name);
	packet send;   //要发送的结构
	packet recv;
	int num = 0;  //发送第几个数据包
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
		if (recv.number == num + 1)         //如果发回来的数据包表明刚刚发送的数据包没有错误
		{
			num++;
			send.length = file.Read(send.data, 1024);
		}
	}
	send.end = true;
	sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
	file.Close();
	timeout = 10000; //平时阻塞时间为10s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	return;
}

// 接收数据并写入文件
void MySocket::RecvData(CString name, sockaddr_in addr_aim, SOCKET server)
{
	int timeout = 2000;      //设置超时时间为2s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	//restart_server(server);
	CString filepath = L"list\\" + name;
	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);       //创建并以二进制打开要下载的文件
	int len = sizeof(addr_aim);
	int num = 0;  //下一个要接受的数据包id
	bool end = false;
	packet send;
	packet recv;
	send.end = false;
	while (!end)
	{
		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
		if (recv.end == true)   //文件接收完成
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
	timeout = 10000; //平时阻塞时间为10s
	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
	user.GetList();        //接受上传文件后重新获取目录
	return;
}