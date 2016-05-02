
// see_serviceDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"


#define WM_UPDATE_SERVICE (WM_USER + 0x1001)

typedef struct _SERVICE_SHOW_PROCESS {
	TCHAR displayName[56];
	TCHAR serviceName[56];
	TCHAR description[8 * 1024];
	TCHAR status[56];
	TCHAR startup_type[56];
}SERVICE_SHOW, *PSERVICE_SHOW;


// Csee_serviceDlg 对话框
class Csee_serviceDlg : public CDialogEx
{
// 构造
public:
	Csee_serviceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SEE_SERVICE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	HANDLE m_hThread = NULL;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedScm(NMHDR *pNMHDR, LRESULT *pResult);
private:
	CListCtrl m_ServiceList;
	afx_msg LRESULT UpdateService(WPARAM wParam, LPARAM lParam);
	static void GetAllService(LPVOID lpParam);
};
