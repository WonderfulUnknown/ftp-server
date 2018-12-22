#include "StdAfx.h"
#include "User.h"


User::User(void)
{
}


User::~User(void)
{
}

CString User::CheckUser(CString username)
{
	CString Pwd = NULL;
	if (file.Open(L"User.txt", CFile::modeRead))
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
	file.Open(L"User.txt", CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
	file.Seek(0, CFile::end);//ָ���ļ���
	CString content = L"User:" + username + L"Pwd:" + password + L"\n";
	file.WriteString(content);
	AfxMessageBox(L"�û���ӳɹ���", MB_ICONINFORMATION);
	file.Close();
}

void User::GetList(void)
{
	file.Open(L"FileList.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	WORD unicode = 0xFEFF; //�ļ�����Unicode��ʽ
	file.Write((void *)&unicode, sizeof(WORD));
	AddFileName(L"File", &filename);
	file.Close();
	filename = filename.Left(filename.GetLength() - 1);
	return;
}

void User::AddFileName(CString FilePath, CString* FileName)
{
	CString start = FilePath + _T("\\*.*");
	CString FullName, Name, content;
	CFileFind finder;

	BOOL exist = finder.FindFile(start);
	while (exist)
	{
		exist = finder.FindNextFile();

		if (finder.IsDots())
			continue;
		//�����Ŀ¼���ݹ���ҵ�Ŀ¼������ļ�
		if (finder.IsDirectory())
			AddFileName(FilePath + L"\\" + finder.GetFileName(), FileName);
		else
		{
			*FileName += finder.GetFileName() + L",";
			content = finder.GetFileName() + L"," + finder.GetFilePath() + _T("\r\n");
			file.Seek(0, CFile::end);
			file.WriteString(content);
			content.Empty();//���
		}
	}
	finder.Close();
}

CString User::GetFilePath(CString Name)
{
	file.Open(L"FileList.txt", CFile::modeRead | CFile::typeBinary);
	WORD unicode = 0xFEFF;//�ļ�����Unicode��ʽ
	file.Read((void *)&unicode, sizeof(WORD));
	CString filepath;
	CString line;
	while (file.ReadString(line))//��ȡһ��
	{
		CString temp = line.Left(line.Find(L","));
		if (temp.Compare(Name) == 0)//���
		{
			filepath = line.Right(line.GetLength() - line.Find(L",") - 1) + L"\n";
			break;
		}
	}
	file.Close();
	filepath = filepath.Left(filepath.Find(L"\r\n"));
	return filepath;
}
