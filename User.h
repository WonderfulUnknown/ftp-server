#pragma once
#include "MySocket.h"
class User
{
public:
	User(void);
	~User(void);

	CStdioFile file;
	CString filename;//����Ŀ¼���ļ���


	//����û�
	void AddUser(CString username, CString password);
	//�ж��û����Ƿ��Ѵ���,���ڷ�������
	CString CheckUser(CString username);
	//��ȡ����Ŀ¼
	void GetList();
	//����ļ���
	void AddFileName(CString FilePath, CString* Filename);
	//��ȡ�ļ�·��
	CString GetFilePath(CString Name);
};

