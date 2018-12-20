#pragma once
#include "MySocket.h"
class User
{
public:
	User(void);
	~User(void);

	CString CheckUser(CString username);//判断用户名是否已存在,存在返回密码
	void AddUser(CString username,CString password);//添加用户
	CStdioFile file;
	CString path;//文件路径
	CString filename;//共享目录下文件名

	//获取共享目录
	void GetList();
	//添加文件名
	void AddFileName(CString strFolderPath, CString* recordstring);

	//获取文件路径
	CString GetFilePath(CString Name);
};

