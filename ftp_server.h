
// ftp_server.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cftp_serverApp: 
// �йش����ʵ�֣������ ftp_server.cpp
//

class Cftp_serverApp : public CWinApp
{
public:
	Cftp_serverApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cftp_serverApp theApp;