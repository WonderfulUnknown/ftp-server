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

void User::GetList(void)
{
	file.Open(L"FileList.txt", CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	WORD unicode = 0xFEFF; //文件采用Unicode格式
	file.Write((void *)&unicode, sizeof(WORD));
	AddFileName(L"File", &filename);
	file.Close();
	filename = filename.Left(filename.GetLength() - 1);
	return;
}

void User::AddFileName(CString strFolderPath, CString* recordstring)
{
	CString strMatch = strFolderPath + _T("\\*.*");
	CString FullName, Name, out;
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strMatch);
	while (bWorking)
	{
		bWorking = finder.FindNextFile();

		if (finder.IsDots())
			continue;
		//如果是目录，递归查找到目录里面的文件
		if (finder.IsDirectory())
			AddFileName(strFolderPath + L"\\" + finder.GetFileName(), recordstring);
		else
		{
			*recordstring += finder.GetFileName() + L",";
			out = finder.GetFileName() + L"," + finder.GetFilePath() + _T("\r\n");
			file.Seek(0, CFile::end);
			file.WriteString(out);
			out.Empty();
		}
	}
	finder.Close();
}

CString User::GetFilePath(CString Name)
{
	file.Open(L"FileList.txt", CFile::modeRead | CFile::typeBinary);
	WORD unicode = 0xFEFF;//文件采用Unicode格式
	file.Read((void *)&unicode, sizeof(WORD));
	CString filepath;
	CString info;
	while (file.ReadString(info))
	{
		CString temp = info.Left(info.Find(L","));
		if (temp.Compare(Name) == 0)
		{
			filepath = info.Right(info.GetLength() - info.Find(L",") - 1) + L"\n";
			break;
		}
	}
	file.Close();
	filepath = filepath.Left(filepath.Find(L"\r\n"));
	return filepath;
}
