// find_service_dll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


typedef struct _SERVICE_SHOW_PROCESS {
	TCHAR displayName[56];
	TCHAR serviceName[56];
	TCHAR description[8 * 1024];
	TCHAR status[56];
	TCHAR startup_type[56];
}SERVICE_SHOW, *PSERVICE_SHOW;

extern "C" __declspec(dllexport)
int GetServiceInformation(int *numberOfService, char* pBuf, char* errBuf)
{
	if (errBuf == NULL || pBuf == NULL)
		return -1;
	SERVICE_STATUS status;
	SC_HANDLE hSCM = NULL;
	SC_HANDLE scService;
	DWORD dwBufSize = 0;
	DWORD dwBufNeed = 0;
	DWORD dwNumberOfService = 0;
	ENUM_SERVICE_STATUS_PROCESS *pServiceInfo = NULL;
	BOOL bRet = FALSE;
	
	memcpy(errBuf, "success!", sizeof("success!"));
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCM == NULL)
	{
		//printf("打开服务管理系统失败!\n");
		memcpy(errBuf, "打开服务管理系统失败!\n", sizeof("打开服务管理系统失败!\n"));
		return FALSE;
	}

	//获取需要的缓冲区大小
	EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO,
		SERVICE_WIN32,
		SERVICE_STATE_ALL,
		NULL, dwBufSize,
		&dwBufNeed,
		&dwNumberOfService,
		NULL,
		NULL);
	// 多设置存放1个服务信息的长度
	dwBufSize = dwBufNeed + sizeof(ENUM_SERVICE_STATUS_PROCESS);
	if(dwBufSize > 128*1024)
	{
		sprintf(errBuf, "pBuf to short, need %d byte.", dwBufSize);
		return FALSE;
	}

	if (pBuf == NULL)
	{
		memcpy(errBuf, "malloc error.\n", sizeof("malloc error.\n"));
		return FALSE;
	}
	char* buf = (char*)malloc(1024 * 256);
	memset(buf, 0, 1024 * 256);
	bRet = EnumServicesStatusEx(hSCM, SC_ENUM_PROCESS_INFO, SERVICE_WIN32, SERVICE_STATE_ALL,
		(LPBYTE)buf, dwBufSize, &dwBufNeed, &dwNumberOfService, NULL, NULL);
	if (bRet == FALSE)
	{
		memcpy(errBuf, "EnumServicesStatusEx error.\n", sizeof("EnumServicesStatusEx error.\n"));
		CloseServiceHandle(hSCM);
		return FALSE;
	}
	SERVICE_SHOW * pServiceShow = (SERVICE_SHOW *)pBuf;
	pServiceInfo = (LPENUM_SERVICE_STATUS_PROCESS)buf;
	SC_HANDLE hService = NULL;
	LPQUERY_SERVICE_CONFIG pServiceConfig = NULL;
	LPSERVICE_DESCRIPTION lpServiceDescription = NULL;
	//char * lpServiceDescription = NULL;
	//lpServiceDescription = (char*)malloc(1024 * 8);
	lpServiceDescription = (LPSERVICE_DESCRIPTION)malloc(1024*8);
	
	memset(lpServiceDescription, 0, 1024 * 8);
	for (int i = 0; i < dwNumberOfService; i++)
	{
		pServiceConfig = NULL;
		//lpServiceDescription = NULL;
		memset(lpServiceDescription, 0, 1024 * 8);
		hService = NULL;
		hService = OpenService(hSCM, pServiceInfo[i].lpServiceName, SERVICE_QUERY_CONFIG);
		QueryServiceConfig(hService, pServiceConfig, 0, &dwBufSize);
		pServiceConfig = (LPQUERY_SERVICE_CONFIG)malloc(dwBufSize);
		QueryServiceConfig(hService, pServiceConfig, dwBufSize, &dwBufSize);
		_tcscpy(pServiceShow[i].displayName, pServiceInfo[i].lpDisplayName);
		_tcscpy(pServiceShow[i].serviceName, pServiceInfo[i].lpServiceName);
		//_tcscpy(pServiceShow[i].discript, L"描述");
		switch(pServiceInfo[i].ServiceStatusProcess.dwCurrentState)
		{
		case SERVICE_STOPPED:
			_tcscpy(pServiceShow[i].status, L"已停止");
			break;
		case SERVICE_STOP_PENDING:
			_tcscpy(pServiceShow[i].status, L"正在停止");
			break;
		case SERVICE_START_PENDING:
			_tcscpy(pServiceShow[i].status, L"正在启动");
			break;
		case SERVICE_RUNNING:
			_tcscpy(pServiceShow[i].status, L"正在运行");
			break;
		case SERVICE_PAUSED:
			_tcscpy(pServiceShow[i].status, L"已暂停");
			break;
		case SERVICE_PAUSE_PENDING:
			_tcscpy(pServiceShow[i].status, L"正在暂停");
			break;
		case SERVICE_CONTINUE_PENDING:
			_tcscpy(pServiceShow[i].status, L"准备继续");
			break;
		default:
			_tcscpy(pServiceShow[i].status, L" ");
			break;
		}
		switch(pServiceConfig->dwStartType)
		{
		case SERVICE_AUTO_START:
			_tcscpy(pServiceShow[i].startup_type, _T("自动启动"));
			break;
		case SERVICE_BOOT_START:
			_tcscpy(pServiceShow[i].startup_type, _T("引导启动"));
			break;
		case SERVICE_DEMAND_START:
			_tcscpy(pServiceShow[i].startup_type, _T("延迟启动"));
			break;
		case SERVICE_DISABLED:
			_tcscpy(pServiceShow[i].startup_type, _T("禁止启动"));
			break;
		case SERVICE_SYSTEM_START:
			_tcscpy(pServiceShow[i].startup_type, _T("跟随系统启动"));
			break;
		default:
			_tcscpy(pServiceShow[i].startup_type, _T(" "));
			break;
		}
		dwBufSize = 1024 * 8;
		//QueryServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpServiceDescription, 0, &dwBufSize);
		lpServiceDescription = (LPSERVICE_DESCRIPTION)malloc(dwBufSize);
		
		QueryServiceConfig2(hService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpServiceDescription, dwBufSize, &dwBufSize);
		if(lpServiceDescription == NULL || lpServiceDescription->lpDescription == NULL)
			_tcscpy(pServiceShow[i].description, L"");
		else
			_tcscpy(pServiceShow[i].description, lpServiceDescription->lpDescription);
		free(pServiceConfig);
		CloseServiceHandle(hService);
		free(lpServiceDescription);
	}
	free(lpServiceDescription);
	//free(lpServiceDescription);
	//free(pServiceConfig);
	
	free(pServiceInfo);
	// 关闭打开的服务句柄
	bRet = CloseServiceHandle(hSCM);
	if (bRet == FALSE)
	{
		//printf("CloseServiceHandle error.\n");
		memcpy(errBuf, "CloseServiceHandle error.\n", sizeof("CloseServiceHandle error.\n"));
	}
	//pServiceInfo = (LPENUM_SERVICE_STATUS_PROCESS)pBuf;
	*numberOfService = dwNumberOfService;
	return TRUE;
}


