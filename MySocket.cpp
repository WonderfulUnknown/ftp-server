#include "stdafx.h"
#include "MySocket.h"
#include "ftp_server.h"
#include "ftp_serverDlg.h"

using namespace std;

MySocket::MySocket()
{

}


MySocket::~MySocket()
{
}


//参数nErrorCode,函数调用时，MFC框架提供，表明套接字最新的状态
//如果是0，函数正常执行，没有错误；非0，套接字对象出错
void MySocket::OnAccept(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	Cftp_serverDlg *dlg = (Cftp_serverDlg*)AfxGetApp()->GetMainWnd();//主窗口指针对象
	CString log;
	//char *msg;
	MySocket *sock = new MySocket();
	if (Accept(*sock))
	{
		//	msg = "220 smtp 127.0.0.1 is ready\r\n";
		//	sock->Send(msg, strlen(msg), 0);
		log = L"TCP连接成功";
		//触发FD_READ事件，调用OnReceive函数
		sock->AsyncSelect(FD_READ);
	}
	else
	{
		log = "TCP连接失败\r\n";
		sock->Close();
	}
	dlg->m_Log.AddString(log);

	CAsyncSocket::OnAccept(nErrorCode);
}


void MySocket::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);
}


void MySocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

//	//每次receive之前需要把缓冲区清零
//	memset(data, 0, sizeof(data));
//	//length存储返回收到消息的长度，接收到的数据存到data中
//	length = Receive(data, sizeof(data), 0);
//
//	//将数据从等待队列读入缓存区，并且不将数据从缓冲区清除
//	//length = Receive(data, sizeof(data), MSG_PEEK);
//	//接受外带数据
//	//length = Receive(data, sizeof(data), MSG_OOB);
//
//	receive = data;
//	CString log;
//	
//	AfxGetMainWnd()->GetDlgItemText(IDC_Log, log);
//	if (!IsData)
//		log = log + L"C:" + receive.Left(length);
//	else
//		log = log + L"C:" + L"data\r\n";
//
//	if (length != SOCKET_ERROR)
//	{
//		if (!IsData)
//		{
//			std::regex mail_pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
//			CString Mail;
//			string mail;
//			smatch r;
//			switch (step)
//			{
//			case 1:
//				//截取左侧n个字符，遇到双字节字符比如中文，则可能会截断乱码，n按照字节计数
//				if (receive.Left(4) == "HELO" || receive.Left(4) == "EHLO")
//					msg = "250 OK hello smtp 127.0.0.1\r\n";
//				else IsError = true;
//				break;
//			case 2:
//				if (receive.Left(10) == "MAIL FROM:")
//				{
//					Mail = receive.Mid(11);//截取邮箱
//					Mail.Remove('<');
//					Mail.Remove('>');
//					Mail.Remove('\r');
//					Mail.Remove('\n');
//					mail = CT2A(Mail.GetBuffer());//将CString转为string
//					//MessageBox(NULL,Mail, L"debug", MB_OK);
//
//					//判断整个表达式是否匹配
//					regex_match(mail, r, mail_pattern);
//					if (r.str() == mail)
//					{
//						msg = "250 Sender's mail is ok\r\n";
//						//msg = "250 Sender: ";
//						//strcat_s比strcat安全，不会溢出
//						//strcat_s(msg, 100, (char*)mail.c_str());//.c_str()将str转换为char *
//						//strcat_s(msg, 100, " OK \r\n");//sizeof(char *)=4,出错
//					}
//					else
//						msg = "550 Mailbox is not available";
//				}
//				else IsError = true;
//				break;
//			case 3:
//				if (receive.Left(8) == "RCPT TO:")
//				{
//					Mail = receive.Mid(11);//截取邮箱
//					Mail.Remove('<');
//					Mail.Remove('>');
//					Mail.Remove('\r');
//					Mail.Remove('\n');
//					mail = CT2A(Mail.GetBuffer());//将CString转为string
//					//MessageBox(NULL,Mail, L"debug", MB_OK);
//
//					//判断整个表达式是否匹配
//					regex_match(mail, r, mail_pattern);
//					if (r.str() == mail)
//						msg = "250 Receiver's mail is ok\r\n";	
//					else
//						msg = "550 Mailbox is not available";
//				}
//				else IsError = true;
//				break;
//			case 4:
//				if (receive.Left(4) == "DATA")
//				{
//					IsData = true;//如果收到DATA命令，说明接下来的接收到的是数据
//					msg = "354 Start mail input,end with <CRLF>.<CRLF>\r\n";
//				}
//				else IsError = true;
//				break;
//			default:
//				if (receive.Left(4) == "QUIT")
//				{
//					msg = "221 smtp 127.0.0.1 server closing connection\r\n";
//					Quit = true;//客户端退出命令，终止程序
//				}
//			}
//			if (IsError)
//				msg = "550 Error: bad syntax\r\n";
//
//			Send(msg, strlen(msg), 0);//发送应答
//
//			step++;
//			log = log + L"S:" + (CString)msg;
//			AsyncSelect(FD_READ);//触发接收函数
//			AfxGetMainWnd()->SetDlgItemText(IDC_Log, log);//写发送日志
//			return;
//		}
//		else
//		{
//			CString temp;
//			//保留之前写好的邮件格式等信息
//			AfxGetMainWnd()->GetDlgItemText(IDC_INFO, temp);
//
//			//获取邮件报文 + 邮件内容base64码
//			content = temp + receive.Left(length);
//			AfxGetMainWnd()->SetDlgItemText(IDC_INFO, content);
//
//			CString text;
//			//CString.Find如果查到，返回以0索引起始的位置;未查到，返回-1
//			if (receive.Find(L"Subject"))
//			{
//				temp = receive.Mid(receive.Find(L"Subject"));
//				//text = temp.Right(temp.GetLength() - temp.Find(L"X-Priority"));
//				text = temp.Left(receive.Find(L"X-Priority"));
//				//text = temp.Left(content.Find(L"\r\n"));
//				text += L"\r\n";
//			}
//			CString data;
//			if (receive.Find(L"Content-Type: text"))
//			{
//				receive = receive.Right(receive.GetLength() - receive.Find(L"base64"));
//				for (int i = 6; receive.GetAt(i) != '-'; i++)
//					data.AppendChar(receive.GetAt(i));
//				data = Decode_base64(data);
//
//				text += data;
//				AfxGetMainWnd()->SetDlgItemText(IDC_Content, text);
//			}
//
//			//<CRLF>.<CRLF>
//			if (receive.Find(L"\r\n.\r\n"))
//			{
//				IsData = false;
//
//				msg = "250 Message accepted\r\n";//发送应答
//				Send(msg, strlen(msg), 0);
//
//				log = log + "S:" + (CString)msg;
//				AfxGetMainWnd()->SetDlgItemText(IDC_Log, log);
//
//				//如果有附件
//				if (content.Find(L"Content-Disposition: attachment"))
//				{
//					int begin = content.Find(L"filename=");
//					begin = content.Find('.', begin);
//					begin = content.Find('"', begin) + 1;
//
//					CString Picture;
//
//					Picture = content.Mid(begin);
//					Decode_base64(Picture);
//				}
//			}
//			AsyncSelect(FD_READ);
//
//			return;
//		}
//	}
//	if (Quit)//退出
//	{
//		MySocket sock;
//		sock.OnClose(0);
//		Quit = false;
//		return;
//	}

	CAsyncSocket::OnReceive(nErrorCode);
}