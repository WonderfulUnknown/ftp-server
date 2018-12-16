#include "StdAfx.h"
#include "User.h"


User::User(void)
{
	path = L"User.txt";
}


User::~User(void)
{
}

CString User::CheckUser(CString username)
{
	CString Pwd = NULL;
	if (file.Open(path, CFile::modeRead))
	{
		CString line;
		while (file.ReadString(line))//读取一行
		{
			CString temp = line.Left(line.Find(L"Pwd:"));
			CString name = temp.Mid(5);
			if (!name.CompareNoCase(username))
			{
				file.Close();
				Pwd = line.Mid(line.Find(L"Pwd:") + 4);
				return Pwd;
			}
		}
		file.Close();
	}
	
	return Pwd;
}

void User::AddUser(CString username, CString password)
{
	if (!CheckUser(username).IsEmpty())
	{
		AfxMessageBox(L"用户名已经存在！", MB_ICONSTOP);
		return;
	}

	//CFile::modeCreate 文件以创建形式打开(创建一个新的文件，老文件数据将会丢失，
	//配合CFile::modeNoTruncate 一起使用，不会覆盖老文件)
	file.Open(path, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	file.Seek(0, CFile::end);//指到文件底
	CString content = L"User:" + username + L"Pwd:" + password + L"\n";
	file.WriteString(content);
	AfxMessageBox(L"用户添加成功！", MB_ICONINFORMATION);
	file.Close();
}

// 获取共享目录
void User::GetList(void)
{
	file.Open(L"List.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	WORD unicodeFlag = 0xFEFF; //文件采用Unicode格式
	file.Write((void *)&unicodeFlag, sizeof(WORD));
	AddFileFromFolder(L"List", &filename);
	file.Close();
	filename = filename.Left(filename.GetLength() - 1);
	return;
}


// 从文件夹里添加文件名
void User::AddFileFromFolder(CString strFolderPath, CString* recordstring)
{
	CString strMatch = strFolderPath + _T("\\*.*");
	CString strFullName;
	CString Name;
	CFileFind finder;
	CString out;
	BOOL bWorking = finder.FindFile(strMatch);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			AddFileFromFolder(strFolderPath + L"\\" + finder.GetFileName(), recordstring);
		}
		else
		{
			Name = finder.GetFileName();
			strFullName = finder.GetFilePath();
			*recordstring += Name + L",";
			out = Name + L"," + strFullName + _T("\r\n");
			file.Seek(0, CFile::end);
			file.WriteString(out);
			out.Empty();
		}
	}
	finder.Close();
}
//
//
//// 运用文件名从文件中获取文件路径
//CString User::GetFilePath(CString Name)
//{
//	file.Open(L"List.txt", CFile::modeRead | CFile::typeBinary);
//	WORD unicodeFlag = 0xFEFF; //文件采用Unicode格式
//	file.Read((void *)&unicodeFlag, sizeof(WORD));
//	CString filepath;
//	CString info;
//	while (file.ReadString(info))
//	{
//		CString temp_name = info.Left(info.Find(L","));
//		if (temp_name.Compare(Name) == 0)
//		{
//			filepath = info.Right(info.GetLength() - info.Find(L",") - 1) + L"\n";
//			break;
//		}
//	}
//	file.Close();
//	filepath = filepath.Left(filepath.Find(L"\r\n"));
//	return filepath;
//}
//
//
////将文件内容读取并发送
//void User::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	restart_server(server);
//	int timeout = 2000;      //设置超时时间为2s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	CString filepath = GetFilePath(name);
//	packet send;   //要发送的结构
//	packet recv;
//	int num = 0;  //发送第几个数据包
//	int len = sizeof(addr_aim);
//	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
//	file.SeekToBegin();
//	int length_packet = file.GetLength();
//	send.data_len = file.Read(send.data, 1024);
//	send.end = false;
//	while (send.data_len)
//	{
//		send.packetid = num;
//		sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
//		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
//		if (recv.packetid == num + 1)         //如果发回来的数据包表明刚刚发送的数据包没有错误
//		{
//			num++;
//			send.data_len = file.Read(send.data, 1024);
//		}
//	}
//	send.end = true;
//	sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
//	file.Close();
//	timeout = 10000; //平时阻塞时间为10s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	return;
//}
//
//// 接收数据并写入文件
//void User::RecvData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	int timeout = 2000;      //设置超时时间为2s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	restart_server(server);
//	CString filepath = L"list\\" + name;
//	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);       //创建并以二进制打开要下载的文件
//	int len = sizeof(addr_aim);
//	int num = 0;  //下一个要接受的数据包id
//	bool end = false;
//	packet send;
//	packet recv;
//	send.end = false;
//	while (!end)
//	{
//		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
//		if (recv.end == true)   //文件接收完成
//		{
//			end = true;
//		}
//		else
//		{
//			if (recv.packetid == num)
//			{
//				recv.data[recv.data_len] = '\0';
//				file.SeekToEnd();
//				file.Write(recv.data, recv.data_len);
//				num++;
//			}
//			strcpy(send.data, "ACK");
//			send.data_len = strlen(send.data);
//			send.packetid = num;
//			sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
//		}
//	}
//	file.Close();
//	timeout = 10000; //平时阻塞时间为10s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	GetList();        //接受上传文件后重新获取目录
//	return;
//}
//
//
//// 重启服务器端套接字，使得对话框的线程退出监听
//void User::restart_server(SOCKET server)
//{
//	closesocket(server);
//	server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//	if (server == INVALID_SOCKET)
//	{
//		return;
//	}
//	sockaddr_in sin;
//	sin.sin_family = AF_INET;
//	sin.sin_port = htons(4567);
//	sin.sin_addr.S_un.S_addr = INADDR_ANY;
//	if (bind(server, (sockaddr *)&sin, sizeof(sockaddr_in)) == SOCKET_ERROR)
//	{
//		return;
//	}
//}

//// 获取共享目录
//void User::GetList()
//{
//	file.Open(L"List.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
//	WORD unicodeFlag = 0xFEFF; //文件采用Unicode格式
//	file.Write((void *)&unicodeFlag, sizeof(WORD));
//	AddFileFromFolder(L"List", &filename);
//	file.Close();
//	filename = filename.Left(filename.GetLength() - 1);
//	return;
//}


//// 从文件夹里添加文件名
//void User::AddFileFromFolder(CString strFolderPath, CString* recordstring)
//{
//	CString strMatch = strFolderPath + _T("\\*.*");
//	CString strFullName;
//	CString Name;
//	CFileFind finder;
//	CString out;
//	BOOL bWorking = finder.FindFile(strMatch);
//	while (bWorking)
//	{
//		bWorking = finder.FindNextFile();
//
//		if (finder.IsDots())
//			continue;
//
//		if (finder.IsDirectory())
//		{
//			AddFileFromFolder(strFolderPath + L"\\" + finder.GetFileName(), recordstring);
//		}
//		else
//		{
//			Name = finder.GetFileName();
//			strFullName = finder.GetFilePath();
//			*recordstring += Name + L",";
//			out = Name + L"," + strFullName + _T("\r\n");
//			file.Seek(0, CFile::end);
//			file.WriteString(out);
//			out.Empty();
//		}
//	}
//	finder.Close();
//}


//// 运用文件名从文件中获取文件路径
//CString User::GetFilePath(CString Name)
//{
//	file.Open(L"List.txt", CFile::modeRead | CFile::typeBinary);
//	WORD unicodeFlag = 0xFEFF; //文件采用Unicode格式
//	file.Read((void *)&unicodeFlag, sizeof(WORD));
//	CString filepath;
//	CString info;
//	while (file.ReadString(info))
//	{
//		CString temp_name = info.Left(info.Find(L","));
//		if (temp_name.Compare(Name) == 0)
//		{
//			filepath = info.Right(info.GetLength() - info.Find(L",") - 1) + L"\n";
//			break;
//		}
//	}
//	file.Close();
//	filepath = filepath.Left(filepath.Find(L"\r\n"));
//	return filepath;
//}
//
//
////将文件内容读取并发送
//void User::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	CString filepath = GetFilePath(name);
//	char data[1023];
//	//CString data;
//	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
//	//WORD unicodeFlag = 0xFEFF; //文件采用Unicode格式
//	//file.Read((void *)&unicodeFlag,sizeof(WORD));
//	CString info;
//	file.SeekToBegin();
//	int i = file.GetLength();
//	while (file.Read(data, 1023))
//	{
//		//data += info;
//		sendto(server, data, strlen(data), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
//	}
//	file.Close();
//	return;
//}
