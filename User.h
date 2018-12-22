#pragma once
#include "MySocket.h"
class User
{
public:
	User(void);
	~User(void);

	CStdioFile file;
	CString filename;//共享目录下文件名


	//添加用户
	void AddUser(CString username, CString password);
	//判断用户名是否已存在,存在返回密码
	CString CheckUser(CString username);
	//获取共享目录
	void GetList();
	//添加文件名
	void AddFileName(CString FilePath, CString* Filename);
	//获取文件路径
	CString GetFilePath(CString Name);
};

