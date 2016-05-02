
// see_serviceDlg.cpp : 实现文件
//


//#include <stdio.h>
//#include <winsvc.h>

#include "stdafx.h"
#include "see_service.h"
#include "see_serviceDlg.h"
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


// Csee_serviceDlg 对话框



Csee_serviceDlg::Csee_serviceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SEE_SERVICE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Csee_serviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, ID_SCM, m_ServiceList);
}

BEGIN_MESSAGE_MAP(Csee_serviceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, ID_SCM, &Csee_serviceDlg::OnLvnItemchangedScm)
	ON_MESSAGE(WM_UPDATE_SERVICE, &Csee_serviceDlg::UpdateService)
END_MESSAGE_MAP()


// Csee_serviceDlg 消息处理程序

BOOL Csee_serviceDlg::OnInitDialog()
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

	CRect rect;
	m_ServiceList.GetClientRect(&rect);

	m_ServiceList.SetExtendedStyle(m_ServiceList.GetExtendedStyle()
		| LVS_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ServiceList.InsertColumn(0, _T("显示名称"), LVCFMT_CENTER, rect.Width()/5, 0);
	m_ServiceList.InsertColumn(1, _T("服务名称"), LVCFMT_CENTER, rect.Width() / 5, 1);
	m_ServiceList.InsertColumn(2, _T("描述"), LVCFMT_CENTER, rect.Width() / 3, 2);
	m_ServiceList.InsertColumn(3, _T("状态"), LVCFMT_CENTER, rect.Width() / 10, 3);
	m_ServiceList.InsertColumn(4, _T("启动类型"), LVCFMT_CENTER, rect.Width() / 10, 4);

	m_hThread = (HANDLE)_beginthread(GetAllService, 0, this);
	if(m_hThread == NULL)
	{
		::MessageBox(NULL, L"获取dll线程创建失败", NULL, NULL);
		exit(-1);
	}
	WaitForSingleObject(m_hThread, 10);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Csee_serviceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Csee_serviceDlg::OnPaint()
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
		CRect rect;
		GetClientRect(&rect);
		m_ServiceList.MoveWindow(rect);
		m_ServiceList.SetColumnWidth(0, rect.Width() / 5);
		m_ServiceList.SetColumnWidth(1, rect.Width() / 5);
		m_ServiceList.SetColumnWidth(2, rect.Width() / 3);
		m_ServiceList.SetColumnWidth(3, rect.Width() / 10);
		m_ServiceList.SetColumnWidth(4, rect.Width() / 10);
		//m_ServiceList.UpdateData(TRUE);
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Csee_serviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Csee_serviceDlg::OnLvnItemchangedScm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

/*
获取dll内容线程
*/

typedef int(*GetService)(int *numOfService, char* pBuf, char* errBuf);

void Csee_serviceDlg::GetAllService(LPVOID lpParam)
{
	Csee_serviceDlg *pThis = (Csee_serviceDlg*)lpParam;
	HWND hWnd = pThis->m_hWnd;


	HMODULE hDll = LoadLibraryA("..\\Release\\find_service_dll.dll");
	int num=0;
	char* pBuf = NULL;
	char* eBuf = NULL;
	//ENUM_SERVICE_STATUS_PROCESS *pServiceInfo = NULL;
	eBuf = (char*)malloc(1024);
	pBuf = (char*)malloc(sizeof(SERVICE_SHOW)*512);
	//pBuf = (char*)
	memset(eBuf, 0, 1024);
	memset(pBuf, 0, sizeof(SERVICE_SHOW) *512);
	if (hDll != NULL)
	{
		GetService getService = (GetService)GetProcAddress(hDll, "GetServiceInformation");
		if (getService != NULL)
		{
			getService(&num, pBuf, eBuf);
			if (pBuf != NULL && num != 0)
			{
				::PostMessage(hWnd, WM_UPDATE_SERVICE, (WPARAM)pBuf, (LPARAM)num);
			}
		}
		FreeLibrary(hDll);
	}
	if (eBuf != NULL)
	{
		free(eBuf);
		eBuf = NULL;
	}
	
}

LRESULT Csee_serviceDlg::UpdateService(WPARAM wParam, LPARAM lParam)
{
	SERVICE_SHOW *pServiceInfo = (SERVICE_SHOW*)wParam;
	int num = (int)lParam;
	if(pServiceInfo == NULL)
	{
		return 0;
	}

	for (unsigned int i = 0; i < num; i++)
	{
		m_ServiceList.InsertItem(i, pServiceInfo[i].displayName);
		m_ServiceList.SetItemText(i, 1, pServiceInfo[i].serviceName);
		m_ServiceList.SetItemText(i, 2, pServiceInfo[i].description);
		m_ServiceList.SetItemText(i, 3, pServiceInfo[i].status);
		m_ServiceList.SetItemText(i, 4, pServiceInfo[i].startup_type);
		//Sleep(1);
	}
	if (pServiceInfo != NULL)
	{
		free(pServiceInfo);
		pServiceInfo = NULL;
	}
	return 0;
}