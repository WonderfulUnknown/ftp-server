
// ftp_serverDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ftp_server.h"
#include "ftp_serverDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cftp_serverDlg 对话框



Cftp_serverDlg::Cftp_serverDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FTP_SERVER_DIALOG, pParent)
	, m_User(_T(""))
	, m_Pwd(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cftp_serverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FileList, m_FileList);
	DDX_Control(pDX, IDC_Log, m_Log);
	DDX_Text(pDX, IDC_User, m_User);
	DDX_Text(pDX, IDC_Pwd, m_Pwd);
}

BEGIN_MESSAGE_MAP(Cftp_serverDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_Add, &Cftp_serverDlg::OnBnClickedAdd)
END_MESSAGE_MAP()


// Cftp_serverDlg 消息处理程序

BOOL Cftp_serverDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	user.GetList();//获取文件列表
	USES_CONVERSION;
	filelist = T2A(user.filename);
	CString List(filelist);
	int index = List.Find(L",");
	while (index != -1)
	{
		m_FileList.AddString(List.Left(index));
		List = List.Right(List.GetLength() - index - 1);
		index = List.Find(L",");
	}
	m_FileList.AddString(List);

	CString log;
	//创建底层套接字句柄
	if (!socket.Create(21, SOCK_DGRAM, FD_READ))
	{
		log = _T("创建Socket失败!");
		socket.Close();
	}
	//UDP不需要监听
	else
		log = _T("FTP服务器准备就绪");
	m_Log.AddString(log);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Cftp_serverDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cftp_serverDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Cftp_serverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cftp_serverDlg::OnBnClickedAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取对话框输入
	UpdateData(TRUE);
	if (m_User.IsEmpty())
		AfxMessageBox(L"请输入用户名！", MB_ICONSTOP);
	else if (m_Pwd.IsEmpty())
		AfxMessageBox(L"请输入密码！", MB_ICONSTOP);
	else
		user.AddUser(m_User, m_Pwd);
}

