#include "stdafx.h"
#include "Service.h"
#include <functional>

CService *CService::instance_ = nullptr;

CService::CService()
{

}

CService::~CService()
{
}

BOOL CService::Install(LPCTSTR serviceName)
{
	TCHAR ServiceFileName[MAX_PATH] = { 0, };
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE ServiceHandle = NULL;

	if (!serviceName)
	{
		printf("service name null ~\n");
		return FALSE;
	}

	ServiceFileName[0] = '"';
	GetModuleFileName(NULL, ServiceFileName + 1, MAX_PATH - 1);
	_tcscat_s(ServiceFileName, _T("\""));

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!ServiceControlManager)
	{
		printf("service manager open fail\n");
		return FALSE;
	}

	ServiceHandle = CreateService(ServiceControlManager,
		serviceName,
		serviceName,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		ServiceFileName,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (!ServiceHandle)
	{
		printf("service create fail\n");
		CloseServiceHandle(ServiceControlManager);
		return FALSE;
	}

	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);
	
	return TRUE;
}

BOOL CService::Uninstall(LPCTSTR serviceName)
{
	SC_HANDLE ServiceControlManager = NULL;
	SC_HANDLE ServiceHandle = NULL;

	printf("starting uninstall\n");
	if (!serviceName)
	{
		printf("service name null ~\n");
		return FALSE;
	}

	ServiceControlManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	if (!ServiceControlManager)
	{
		printf("service manager open fail\n");
		return FALSE;
	}

	ServiceHandle = OpenService(ServiceControlManager, serviceName, DELETE);

	if (!ServiceHandle)
	{
		printf("service open fail\n");
		CloseServiceHandle(ServiceControlManager);
		return FALSE;
	}

	if (!DeleteService(ServiceHandle))
	{
		printf("service delete faile\n");
		CloseServiceHandle(ServiceHandle);
		CloseServiceHandle(ServiceControlManager);
		
		return FALSE;
	}

	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);

	return TRUE;
}

BOOL CService::Begin(LPTSTR serviceName)
{
	if(!serviceName)
		return FALSE;

	instance_ = this;

	SERVICE_TABLE_ENTRY DispatchTable[] = 
	{
		{serviceName, RunCallback}, {NULL, NULL}
	};

	_tcscpy_s(mServiceName, sizeof(serviceName), serviceName);
	_tprintf(_T("test"));
	OnStarted();
	
	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		_tprintf(_T("## Debuf console mode ##\n"));
		getchar();
	}
	return TRUE;
}

BOOL CService::End(VOID)
{
	OnStopped();

	return TRUE;
}

VOID CService::RunCallback(DWORD ArgumentCount, LPTSTR *arguments)
{
	DWORD Status;
	instance_->mServiceStatus.dwServiceType = SERVICE_WIN32;
	instance_->mServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	instance_->mServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;

	instance_->mServiceStatus.dwWin32ExitCode = 0;
	instance_->mServiceStatus.dwServiceSpecificExitCode = 0;
	instance_->mServiceStatus.dwCheckPoint = 0;
	instance_->mServiceStatus.dwWaitHint = 0;

	instance_->mServiceStatusHandle = RegisterServiceCtrlHandler(instance_->mServiceName, CtrlHandlerCallback);

	if (instance_->mServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)
		return;

	Status = NO_ERROR;

	instance_->mServiceStatus.dwCurrentState = SERVICE_RUNNING;
	instance_->mServiceStatus.dwCheckPoint = 0;
	instance_->mServiceStatus.dwWaitHint = 0;

	if (!SetServiceStatus(instance_->mServiceStatusHandle, &instance_->mServiceStatus))
		return;
}

VOID CService::CtrlHandlerCallback(DWORD opCode)
{
	switch (opCode)
	{
	case SERVICE_CONTROL_PAUSE:
		instance_->mServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		instance_->mServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		instance_->mServiceStatus.dwWin32ExitCode = 0;
		instance_->mServiceStatus.dwCurrentState = SERVICE_STOPPED;
		instance_->mServiceStatus.dwCheckPoint = 0;
		instance_->mServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	}

	if (!SetServiceStatus(instance_->mServiceStatusHandle, &instance_->mServiceStatus))
		return;
}