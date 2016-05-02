
// see_serviceDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Csee_serviceDlg �Ի���



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


// Csee_serviceDlg ��Ϣ�������

BOOL Csee_serviceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CRect rect;
	m_ServiceList.GetClientRect(&rect);

	m_ServiceList.SetExtendedStyle(m_ServiceList.GetExtendedStyle()
		| LVS_REPORT | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ServiceList.InsertColumn(0, _T("��ʾ����"), LVCFMT_CENTER, rect.Width()/5, 0);
	m_ServiceList.InsertColumn(1, _T("��������"), LVCFMT_CENTER, rect.Width() / 5, 1);
	m_ServiceList.InsertColumn(2, _T("����"), LVCFMT_CENTER, rect.Width() / 3, 2);
	m_ServiceList.InsertColumn(3, _T("״̬"), LVCFMT_CENTER, rect.Width() / 10, 3);
	m_ServiceList.InsertColumn(4, _T("��������"), LVCFMT_CENTER, rect.Width() / 10, 4);

	m_hThread = (HANDLE)_beginthread(GetAllService, 0, this);
	if(m_hThread == NULL)
	{
		::MessageBox(NULL, L"��ȡdll�̴߳���ʧ��", NULL, NULL);
		exit(-1);
	}
	WaitForSingleObject(m_hThread, 10);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Csee_serviceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// ����ͼ��
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Csee_serviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Csee_serviceDlg::OnLvnItemchangedScm(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

/*
��ȡdll�����߳�
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