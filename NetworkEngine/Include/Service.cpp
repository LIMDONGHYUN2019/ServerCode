#include "Service.h"

CService* g_pService;

CService::CService()
{
	memset(m_strName, 0, sizeof(TCHAR) * MAX_PATH);
	g_pService = this;
}

CService::~CService()
{
}

bool CService::Install(const TCHAR* pName)
{
	TCHAR	strServiceFileName[MAX_PATH] = {};
	SC_HANDLE	ServiceControlManager = nullptr;
	SC_HANDLE	ServiceHandle = nullptr;

	if (!pName)
		return false;

	strServiceFileName[0] = '"';
	GetModuleFileName(nullptr, strServiceFileName + 1, MAX_PATH - 1);
	_tcscat_s(strServiceFileName, TEXT("\""));

	ServiceControlManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
	//OpenSCManager (������ ��ǻ��, ���� DB�̸�, �׼��� ����) SC_HANDLE ��ȯ
	if (!ServiceControlManager)
		return false;

	ServiceHandle = CreateService(ServiceControlManager,
		pName, pName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		strServiceFileName,
		nullptr, nullptr, nullptr, nullptr, nullptr);

	if (!ServiceHandle)
	{
		CloseServiceHandle(ServiceControlManager);
		return false;
	}

	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);

	return true;
}

bool CService::UnInstall(const TCHAR* pName)
{
	SC_HANDLE	ServiceControlManager = nullptr;
	SC_HANDLE	ServiceHandle = nullptr;

	if (!pName)
		return false;

	ServiceControlManager = OpenSCManager(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);

	if (!ServiceControlManager)
		return false;

	ServiceHandle = OpenService(ServiceControlManager, pName, DELETE);
	// (sc�ڵ�, ���� �̸�, �׼��� ����)
	if (!ServiceHandle)
	{
		CloseServiceHandle(ServiceControlManager);
		return false;
	}

	if (!DeleteService(ServiceHandle))
	{
		CloseServiceHandle(ServiceHandle);
		CloseServiceHandle(ServiceControlManager);

		return false;
	}

	CloseServiceHandle(ServiceHandle);
	CloseServiceHandle(ServiceControlManager);

	return true;
}

bool CService::Begin(const TCHAR* pName)
{
	if (!pName)
		return false;

	SERVICE_TABLE_ENTRY	DispatchTable[] =
	{
		{const_cast<TCHAR*>(pName), (LPSERVICE_MAIN_FUNCTIONW)CService::RunCallbackStatic},
		//������ ���� LPSERVICE_MAIN_FUNCTIONW �־���
		{nullptr, nullptr}
	};
	//SERVICE_TABLE_ENTRY -> ����ü�μ� ���� ���μ������� ����� ���� �̸�, �����Լ������� ������
	_tcscat_s(m_strName, pName);

	OnStart();

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		_tprintf(TEXT("Debug Console\n"));
		getchar();
	}// ���� ���μ������ο��� ���� �����Լ��ּҸ� ����ϴ� �Լ�

	return true;
}

bool CService::End()
{
	OnStop();

	return true;
}

void CService::RunCallback(DWORD iArgCount, LPTSTR* pArgs)
{
	m_ServiceStatus.dwServiceType = SERVICE_WIN32;
	m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
	m_ServiceStatus.dwWin32ExitCode = 0;
	m_ServiceStatus.dwServiceSpecificExitCode = 0;
	m_ServiceStatus.dwCheckPoint = 0;
	m_ServiceStatus.dwWaitHint = 0;

	m_ServiceHandle = RegisterServiceCtrlHandler(m_strName, (LPHANDLER_FUNCTION)CService::ControlHandleCallbackStatic);
	//��ȯ ������ ���� -> LPHANDLER_FUNCTION �־���
	
	if (m_ServiceHandle == (SERVICE_STATUS_HANDLE)0)
		return;

	m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	m_ServiceStatus.dwCheckPoint = 0;
	m_ServiceStatus.dwWaitHint = 0;

	if (!SetServiceStatus(m_ServiceHandle, &m_ServiceStatus))
		return;
}

void CService::ControlHandleCallback(DWORD iOpCode)
{
	switch (iOpCode)
	{
	case SERVICE_CONTROL_PAUSE:
		m_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
		m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		m_ServiceStatus.dwWin32ExitCode = 0;
		m_ServiceStatus.dwCheckPoint = 0;
		m_ServiceStatus.dwWaitHint = 0;
		break;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	}

	if (!SetServiceStatus(m_ServiceHandle, &m_ServiceStatus))
		return;
}

void CService::RunCallbackStatic(DWORD iArgCount, LPTSTR* pArgs)
{
	g_pService->RunCallback(iArgCount, pArgs);
}

void CService::ControlHandleCallbackStatic(DWORD iOpCode)
{
	g_pService->ControlHandleCallback(iOpCode);
}
