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
		while (file.ReadString(line))//��ȡһ��
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
		AfxMessageBox(L"�û����Ѿ����ڣ�", MB_ICONSTOP);
		return;
	}

	//CFile::modeCreate �ļ��Դ�����ʽ��(����һ���µ��ļ������ļ����ݽ��ᶪʧ��
	//���CFile::modeNoTruncate һ��ʹ�ã����Ḳ�����ļ�)
	file.Open(path, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	file.Seek(0, CFile::end);//ָ���ļ���
	CString content = L"User:" + username + L"Pwd:" + password + L"\n";
	file.WriteString(content);
	AfxMessageBox(L"�û���ӳɹ���", MB_ICONINFORMATION);
	file.Close();
}

// ��ȡ����Ŀ¼
void User::GetList(void)
{
	file.Open(L"List.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	WORD unicodeFlag = 0xFEFF; //�ļ�����Unicode��ʽ
	file.Write((void *)&unicodeFlag, sizeof(WORD));
	AddFileFromFolder(L"List", &filename);
	file.Close();
	filename = filename.Left(filename.GetLength() - 1);
	return;
}


// ���ļ���������ļ���
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
//// �����ļ������ļ��л�ȡ�ļ�·��
//CString User::GetFilePath(CString Name)
//{
//	file.Open(L"List.txt", CFile::modeRead | CFile::typeBinary);
//	WORD unicodeFlag = 0xFEFF; //�ļ�����Unicode��ʽ
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
////���ļ����ݶ�ȡ������
//void User::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	restart_server(server);
//	int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	CString filepath = GetFilePath(name);
//	packet send;   //Ҫ���͵Ľṹ
//	packet recv;
//	int num = 0;  //���͵ڼ������ݰ�
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
//		if (recv.packetid == num + 1)         //��������������ݰ������ոշ��͵����ݰ�û�д���
//		{
//			num++;
//			send.data_len = file.Read(send.data, 1024);
//		}
//	}
//	send.end = true;
//	sendto(server, (char*)&send, sizeof(send), 0, (sockaddr*)&addr_aim, sizeof(addr_aim));
//	file.Close();
//	timeout = 10000; //ƽʱ����ʱ��Ϊ10s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	return;
//}
//
//// �������ݲ�д���ļ�
//void User::RecvData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	int timeout = 2000;      //���ó�ʱʱ��Ϊ2s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	restart_server(server);
//	CString filepath = L"list\\" + name;
//	file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);       //�������Զ����ƴ�Ҫ���ص��ļ�
//	int len = sizeof(addr_aim);
//	int num = 0;  //��һ��Ҫ���ܵ����ݰ�id
//	bool end = false;
//	packet send;
//	packet recv;
//	send.end = false;
//	while (!end)
//	{
//		recvfrom(server, (char*)&recv, sizeof(recv), 0, (sockaddr*)&addr_aim, &len);
//		if (recv.end == true)   //�ļ��������
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
//	timeout = 10000; //ƽʱ����ʱ��Ϊ10s
//	setsockopt(server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(int));
//	GetList();        //�����ϴ��ļ������»�ȡĿ¼
//	return;
//}
//
//
//// �������������׽��֣�ʹ�öԻ�����߳��˳�����
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

//// ��ȡ����Ŀ¼
//void User::GetList()
//{
//	file.Open(L"List.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
//	WORD unicodeFlag = 0xFEFF; //�ļ�����Unicode��ʽ
//	file.Write((void *)&unicodeFlag, sizeof(WORD));
//	AddFileFromFolder(L"List", &filename);
//	file.Close();
//	filename = filename.Left(filename.GetLength() - 1);
//	return;
//}


//// ���ļ���������ļ���
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


//// �����ļ������ļ��л�ȡ�ļ�·��
//CString User::GetFilePath(CString Name)
//{
//	file.Open(L"List.txt", CFile::modeRead | CFile::typeBinary);
//	WORD unicodeFlag = 0xFEFF; //�ļ�����Unicode��ʽ
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
////���ļ����ݶ�ȡ������
//void User::SendData(CString name, sockaddr_in addr_aim, SOCKET server)
//{
//	CString filepath = GetFilePath(name);
//	char data[1023];
//	//CString data;
//	file.Open(filepath, CFile::modeRead | CFile::typeBinary);
//	//WORD unicodeFlag = 0xFEFF; //�ļ�����Unicode��ʽ
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
