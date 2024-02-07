#pragma once

#include "Engine.h"

class CService
{
public:
	CService();
	virtual ~CService();

public:
	SERVICE_STATUS	m_ServiceStatus;
	SERVICE_STATUS_HANDLE	m_ServiceHandle;
	TCHAR		m_strName[MAX_PATH];

public:
	virtual void OnStart() = 0;
	virtual void OnStop() = 0;

public:
	bool Install(const TCHAR* pName);
	bool UnInstall(const TCHAR* pName);
	bool Begin(const TCHAR* pName);
	bool End();
	void RunCallback(DWORD iArgCount, LPTSTR* pArgs);
	void ControlHandleCallback(DWORD iOpCode);

public:
	static void RunCallbackStatic(DWORD iArgCount, LPTSTR* pArgs);
	static void ControlHandleCallbackStatic(DWORD iOpCode);
};

