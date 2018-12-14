
// ftp_serverDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "MySocket.h"
#include "User.h"

// Cftp_serverDlg �Ի���
class Cftp_serverDlg : public CDialogEx
{
// ����
public:
	Cftp_serverDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTP_SERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	MySocket MySock;
	User user;
	CListBox m_FileList;
	CListBox m_Log;
	CString m_User;
	CString m_Pwd;
	afx_msg void OnBnClickedAdd();
};
