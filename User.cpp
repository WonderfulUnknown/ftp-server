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

