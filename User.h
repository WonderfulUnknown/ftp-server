#pragma once
#include "MySocket.h"
class User
{
public:
	User(void);
	~User(void);

	CString CheckUser(CString username);//�ж��û����Ƿ��Ѵ���,���ڷ�������
	void AddUser(CString username,CString password);//����û�
	CStdioFile file;
	CString path;//�ļ�·��
	CString filename;//����Ŀ¼���ļ���

	// ��ȡ����Ŀ¼
	void GetList();
	// ���ļ���������ļ���
	void AddFileFromFolder(CString strFolderPath, CString* recordstring);

	// �����ļ������ļ��л�ȡ�ļ�·��
	CString GetFilePath(CString Name);
};

