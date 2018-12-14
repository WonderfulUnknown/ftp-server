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
