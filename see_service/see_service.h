
// see_service.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Csee_serviceApp: 
// �йش����ʵ�֣������ see_service.cpp
//

class Csee_serviceApp : public CWinApp
{
public:
	Csee_serviceApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Csee_serviceApp theApp;